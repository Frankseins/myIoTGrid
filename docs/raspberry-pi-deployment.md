# myIoTGrid Raspberry Pi Deployment Guide

**Version:** 1.0
**Letzte Aktualisierung:** 10. Dezember 2025

---

## Inhaltsverzeichnis

1. [Voraussetzungen](#voraussetzungen)
2. [Erstinstallation](#erstinstallation)
3. [Update durchführen](#update-durchführen)
4. [Troubleshooting](#troubleshooting)
5. [Nützliche Befehle](#nützliche-befehle)

---

## Voraussetzungen

### Hardware
- Raspberry Pi 4 (4GB+ RAM empfohlen) oder Raspberry Pi 5
- SD-Karte mit mindestens 32GB
- Stabile Stromversorgung (5V/3A)
- Netzwerkverbindung (Ethernet empfohlen)

### Software
- Ubuntu Server 24.04+ oder Raspberry Pi OS (64-bit)
- Docker und Docker Compose installiert

### Docker Installation (falls noch nicht vorhanden)

```bash
# Docker installieren
curl -fsSL https://get.docker.com | sh

# Benutzer zur Docker-Gruppe hinzufügen
sudo usermod -aG docker $USER

# Neu einloggen oder:
newgrp docker

# Docker Compose Plugin prüfen
docker compose version
```

---

## Erstinstallation

### Schritt 1: Projektverzeichnis erstellen

```bash
mkdir -p ~/myiotgrid
cd ~/myiotgrid
```

### Schritt 2: Docker Compose Dateien herunterladen

```bash
# Hub Stack (API, Frontend, Sensor-Simulator)
curl -fsSL https://raw.githubusercontent.com/Frankseins/myIoTGrid/main/docker-compose.rpi.yml -o docker-compose.yml

# LoRaWAN Stack (ChirpStack, Gateway Bridge, MQTT)
curl -fsSL https://raw.githubusercontent.com/Frankseins/myIoTGrid/main/docker-compose.lorawan.yml -o docker-compose.lorawan.yml
```

### Schritt 3: Konfigurationsdateien erstellen

```bash
# Verzeichnisse erstellen
mkdir -p ~/myiotgrid/config/{chirpstack,chirpstack-gateway-bridge,mosquitto,postgres}
```

#### 3.1 Mosquitto Konfiguration

```bash
cat > ~/myiotgrid/config/mosquitto/mosquitto.conf << 'EOF'
# MQTT Broker Configuration for myIoTGrid
persistence true
persistence_location /mosquitto/data/
log_dest stdout
log_type all

# Listener for ChirpStack (internal)
listener 1883
protocol mqtt
allow_anonymous true

# Listener for external clients (Hub API)
listener 1884
protocol mqtt
allow_anonymous true

# WebSocket listener (optional)
listener 9001
protocol websockets
allow_anonymous true
EOF
```

#### 3.2 PostgreSQL Init Script

```bash
cat > ~/myiotgrid/config/postgres/init-extensions.sql << 'EOF'
CREATE EXTENSION IF NOT EXISTS pg_trgm;
EOF
```

#### 3.3 ChirpStack Konfiguration

```bash
cat > ~/myiotgrid/config/chirpstack/chirpstack.toml << 'EOF'
[logging]
  level = "info"

[postgresql]
  dsn = "postgres://chirpstack:chirpstack@postgres/chirpstack?sslmode=disable"
  max_open_connections = 10
  min_idle_connections = 0

[redis]
  servers = ["redis://redis:6379"]
  tls_enabled = false
  cluster = false

[network]
  net_id = "000000"
  enabled_regions = ["eu868"]

[api]
  bind = "0.0.0.0:8080"
  secret = "myiotgrid-chirpstack-secret-2025"

[gateway]
  allow_unknown_gateways = true

[integration]
  enabled = ["mqtt"]

  [integration.mqtt]
    server = "tcp://mosquitto:1883/"
    json = true
    qos = 0
    clean_session = false
    client_id = "chirpstack"

[monitoring]
  bind = "0.0.0.0:8070"

[[regions]]
  name = "eu868"
  common_name = "EU868"

  [regions.gateway]
    force_gws_private = false

    [regions.gateway.backend]
      enabled = "mqtt"

      [regions.gateway.backend.mqtt]
        server = "tcp://mosquitto:1883/"
        qos = 0
        clean_session = false
        client_id = "chirpstack-eu868-gw"

  [regions.network]
    installation_margin = 10
    rx1_delay = 1
    rx1_dr_offset = 0
    rx2_dr = 0
    rx2_frequency = 869525000
    downlink_tx_power = -1
    adr_disabled = false
    min_dr = 0
    max_dr = 5
EOF
```

#### 3.4 ChirpStack Gateway Bridge Konfiguration

```bash
cat > ~/myiotgrid/config/chirpstack-gateway-bridge/chirpstack-gateway-bridge.toml << 'EOF'
[general]
log_level = 4

[backend]
type = "semtech_udp"

  [backend.semtech_udp]
  udp_bind = "0.0.0.0:1700"

[integration]
marshaler = "protobuf"

  [integration.mqtt]
  event_topic_template = "eu868/gateway/{{ .GatewayID }}/event/{{ .EventType }}"
  command_topic_template = "eu868/gateway/{{ .GatewayID }}/command/#"
  state_topic_template = "eu868/gateway/{{ .GatewayID }}/state/{{ .StateType }}"
  server = "tcp://mosquitto:1883/"
  clean_session = false
EOF
```

### Schritt 4: Konfiguration überprüfen

```bash
# Alle Dateien auflisten
ls -la ~/myiotgrid/config/*/

# Erwartete Ausgabe:
# config/chirpstack/chirpstack.toml
# config/chirpstack-gateway-bridge/chirpstack-gateway-bridge.toml
# config/mosquitto/mosquitto.conf
# config/postgres/init-extensions.sql
```

### Schritt 5: LoRaWAN Stack starten (ZUERST!)

```bash
cd ~/myiotgrid

# LoRaWAN Stack starten
docker compose -f docker-compose.lorawan.yml up -d

# Warten bis alle Services healthy sind (ca. 30-60 Sekunden)
echo "Warte auf Services..."
sleep 30

# Status prüfen
docker compose -f docker-compose.lorawan.yml ps
```

**Erwartete Ausgabe (alle "healthy"):**
```
NAME                                    STATUS
myiotgrid-lorawan-mosquitto            Up (healthy)
myiotgrid-lorawan-redis                Up (healthy)
myiotgrid-lorawan-postgres             Up (healthy)
myiotgrid-lorawan-chirpstack           Up (healthy)
myiotgrid-lorawan-bridge               Up (healthy)
myiotgrid-lorawan-bridge-service       Up (healthy)
```

### Schritt 6: Hub Stack starten

```bash
# Hub Stack starten (verbindet sich mit LoRaWAN Mosquitto)
docker compose up -d

# Status prüfen
docker compose ps
```

**Erwartete Ausgabe:**
```
NAME                        STATUS
myiotgrid-hub-api          Up (healthy)
myiotgrid-hub-frontend     Up
myiotgrid-sensor-sim       Up
```

### Schritt 7: Installation verifizieren

```bash
# Alle Container anzeigen
docker ps --format "table {{.Names}}\t{{.Status}}\t{{.Ports}}"

# Hub API testen
curl -s http://localhost:5002/health | jq .

# ChirpStack Web UI öffnen
echo "ChirpStack UI: http://$(hostname -I | awk '{print $1}'):8080"
echo "Login: admin / admin"

# Hub Frontend öffnen
echo "Hub Frontend: http://$(hostname -I | awk '{print $1}'):80"
```

---

## Update durchführen

### Schnelles Update (empfohlen)

```bash
cd ~/myiotgrid

# 1. Neueste Images herunterladen
docker compose pull
docker compose -f docker-compose.lorawan.yml pull

# 2. Container mit neuen Images neu starten
docker compose -f docker-compose.lorawan.yml up -d
docker compose up -d

# 3. Alte, ungenutzte Images entfernen
docker image prune -f
```

### Vollständiges Update (bei größeren Änderungen)

```bash
cd ~/myiotgrid

# 1. Alle Container stoppen
docker compose down
docker compose -f docker-compose.lorawan.yml down

# 2. Neueste Images herunterladen
docker compose pull
docker compose -f docker-compose.lorawan.yml pull

# 3. Docker Compose Dateien aktualisieren (optional)
curl -fsSL https://raw.githubusercontent.com/Frankseins/myIoTGrid/main/docker-compose.rpi.yml -o docker-compose.yml
curl -fsSL https://raw.githubusercontent.com/Frankseins/myIoTGrid/main/docker-compose.lorawan.yml -o docker-compose.lorawan.yml

# 4. Stacks neu starten (Reihenfolge wichtig!)
docker compose -f docker-compose.lorawan.yml up -d
sleep 30
docker compose up -d

# 5. Aufräumen
docker image prune -f
docker volume prune -f  # Nur wenn keine wichtigen Daten in Volumes!
```

### Einzelnes Image aktualisieren

```bash
# Beispiel: Nur Hub API aktualisieren
docker pull ghcr.io/frankseins/myiotgrid-hub-api:latest
docker compose up -d hub-api

# Beispiel: Nur LoRaWAN Bridge aktualisieren
docker pull ghcr.io/frankseins/myiotgrid-gateway-lorawan:latest
docker compose -f docker-compose.lorawan.yml up -d bridge
```

### Update-Script (automatisiert)

Erstelle ein Update-Script für einfache Updates:

```bash
cat > ~/myiotgrid/update.sh << 'EOF'
#!/bin/bash
set -e

echo "=== myIoTGrid Update ==="
cd ~/myiotgrid

echo "1. Pulling latest images..."
docker compose pull
docker compose -f docker-compose.lorawan.yml pull

echo "2. Restarting LoRaWAN stack..."
docker compose -f docker-compose.lorawan.yml up -d

echo "3. Waiting for LoRaWAN services..."
sleep 30

echo "4. Restarting Hub stack..."
docker compose up -d

echo "5. Cleaning up old images..."
docker image prune -f

echo "6. Status check..."
docker ps --format "table {{.Names}}\t{{.Status}}"

echo ""
echo "=== Update complete! ==="
EOF

chmod +x ~/myiotgrid/update.sh
```

Dann einfach ausführen:
```bash
~/myiotgrid/update.sh
```

---

## Troubleshooting

### Problem: Container startet nicht / "unhealthy"

```bash
# Logs des betroffenen Containers prüfen
docker logs myiotgrid-lorawan-chirpstack --tail=50
docker logs myiotgrid-hub-api --tail=50

# Detaillierter Status
docker inspect myiotgrid-lorawan-chirpstack | jq '.[0].State'
```

### Problem: "Is a directory" Fehler bei Konfigdateien

Eine Konfigurationsdatei wurde als Verzeichnis erstellt.

```bash
# Betroffene Datei/Verzeichnis löschen
sudo rm -rf ~/myiotgrid/config/mosquitto/mosquitto.conf

# Neu erstellen (als Datei)
cat > ~/myiotgrid/config/mosquitto/mosquitto.conf << 'EOF'
# ... Inhalt ...
EOF

# Prüfen ob es eine Datei ist (zeigt "-rw-r--r--", nicht "d")
ls -la ~/myiotgrid/config/mosquitto/
```

### Problem: PostgreSQL pg_trgm Extension fehlt

```bash
# Symptom: ChirpStack Logs zeigen:
# "operator class gin_trgm_ops does not exist"

# Lösung: PostgreSQL Volume löschen und neu starten
docker compose -f docker-compose.lorawan.yml down
docker volume rm myiotgrid-lorawan-postgres-data

# Init-Script prüfen
cat ~/myiotgrid/config/postgres/init-extensions.sql
# Muss enthalten: CREATE EXTENSION IF NOT EXISTS pg_trgm;

# Neu starten
docker compose -f docker-compose.lorawan.yml up -d
```

### Problem: Hub kann Mosquitto nicht erreichen

```bash
# Prüfen ob Mosquitto läuft
docker ps | grep mosquitto

# Mosquitto Logs prüfen
docker logs myiotgrid-lorawan-mosquitto --tail=20

# Von Hub aus testen (im Container)
docker exec myiotgrid-hub-api sh -c "nc -zv host.docker.internal 1884"
```

### Problem: Datenbank korrupt / DbUpdateConcurrencyException

```bash
# Hub Datenbank zurücksetzen
docker compose down
docker volume rm myiotgrid-hub-data
docker compose up -d
```

### Problem: Speicherplatz voll

```bash
# Docker aufräumen
docker system prune -a -f
docker volume prune -f

# Speicherplatz prüfen
df -h
```

---

## Nützliche Befehle

### Status & Monitoring

```bash
# Alle Container anzeigen
docker ps -a

# Nur myIoTGrid Container
docker ps --format "table {{.Names}}\t{{.Status}}\t{{.Ports}}" | grep myiotgrid

# Live Logs folgen
docker logs -f myiotgrid-hub-api
docker logs -f myiotgrid-lorawan-chirpstack

# Ressourcenverbrauch
docker stats --no-stream
```

### Starten & Stoppen

```bash
# Alles stoppen
docker compose down
docker compose -f docker-compose.lorawan.yml down

# Alles starten (Reihenfolge wichtig!)
docker compose -f docker-compose.lorawan.yml up -d && sleep 30 && docker compose up -d

# Einzelnen Container neu starten
docker compose restart hub-api
docker compose -f docker-compose.lorawan.yml restart chirpstack
```

### Daten & Backups

```bash
# Volumes auflisten
docker volume ls | grep myiotgrid

# SQLite Datenbank sichern
docker cp myiotgrid-hub-api:/app/data/hub.db ~/backups/hub-$(date +%Y%m%d).db

# PostgreSQL Datenbank sichern
docker exec myiotgrid-lorawan-postgres pg_dump -U chirpstack chirpstack > ~/backups/chirpstack-$(date +%Y%m%d).sql
```

### Netzwerk

```bash
# Docker Netzwerke anzeigen
docker network ls | grep myiotgrid

# IP-Adressen der Container
docker network inspect myiotgrid-lorawan-internal

# Port-Belegung prüfen
sudo netstat -tlnp | grep -E "(1883|1884|5002|8080)"
```

---

## Service-Übersicht

| Service | Port | URL | Beschreibung |
|---------|------|-----|--------------|
| Hub API | 5002 | http://IP:5002 | REST API für Sensordaten |
| Hub Frontend | 80 | http://IP | Web-Dashboard |
| ChirpStack | 8080 | http://IP:8080 | LoRaWAN Network Server UI |
| MQTT (intern) | 1883 | tcp://IP:1883 | Für ChirpStack |
| MQTT (extern) | 1884 | tcp://IP:1884 | Für Hub API |
| Gateway Bridge | 1700/udp | udp://IP:1700 | LoRa Gateway Packets |
| Bridge Service | 5100 | http://IP:5100 | LoRaWAN-Hub Bridge |

---

## Default Zugangsdaten

| Service | Benutzer | Passwort |
|---------|----------|----------|
| ChirpStack | admin | admin |
| PostgreSQL | chirpstack | chirpstack |

**Wichtig:** Ändern Sie die Passwörter in Produktionsumgebungen!

---

## Support

- GitHub Issues: https://github.com/Frankseins/myIoTGrid/issues
- Dokumentation: https://mysocialcare-doku.atlassian.net/wiki/spaces/myIoTGrid

---

*myIoTGrid - Open Source IoT Platform*
