# myIoTGrid - Docker Deployment

Diese Dokumentation beschreibt, wie du die myIoTGrid Docker-Container erstellst und startest.

## Übersicht

myIoTGrid besteht aus 4 Docker-Containern:

| Container | Beschreibung | Port | Protokoll |
|-----------|--------------|------|-----------|
| `hub-api` | Backend API (.NET 10) | 5001 | HTTPS |
| `hub-api` | Hub Discovery Service | 5002 | UDP |
| `hub-frontend` | Frontend (Angular + nginx) | 443 | HTTPS |
| `mosquitto` | MQTT Broker | 1883, 9001 | TCP/WS |
| `sensor-sim` | Test-Sensor Simulator | - | intern |

## Voraussetzungen

- Docker Desktop (Mac/Windows) oder Docker Engine (Linux)
- Docker Compose v2+
- ca. 4 GB freier Speicher

## Schnellstart

```bash
# 1. Repository klonen (falls noch nicht geschehen)
git clone https://github.com/frankseins/myIoTGrid.git
cd myIoTGrid

# 2. Alle Docker Images bauen
./build-docker.sh

# 3. Stack starten
docker compose up -d

# 4. Status prüfen
docker compose ps
```

## Zugriff

Nach dem Start sind folgende URLs verfügbar:

| Service | URL |
|---------|-----|
| Frontend | https://localhost |
| Backend API | https://localhost:5001 |
| API Health | https://localhost:5001/health |
| MQTT | localhost:1883 |
| MQTT WebSocket | localhost:9001 |

> **Hinweis:** Die Container verwenden selbstsignierte Zertifikate. Browser zeigen eine Sicherheitswarnung - das ist im Development normal.

## Docker Images bauen

### Option 1: Build-Script (empfohlen)

```bash
# Alle Images bauen
./build-docker.sh

# Mit spezifischem Tag
./build-docker.sh --tag v1.0.0

# Ohne Cache (vollständiger Rebuild)
./build-docker.sh --no-cache

# Bauen und zu Registry pushen
./build-docker.sh --push
```

### Option 2: Docker Compose

```bash
# Alle Services bauen
docker compose build

# Einzelnen Service bauen
docker compose build hub-api
docker compose build hub-frontend
docker compose build sensor-sim
```

### Option 3: Einzelne Images

```bash
# Backend API
docker build -t myiotgrid/hub-api:latest \
  -f myIoTGrid.Hub/myIoTGrid.Hub.Backend/src/myIoTGrid.Hub.Api/Dockerfile \
  myIoTGrid.Hub/myIoTGrid.Hub.Backend

# Frontend
docker build -t myiotgrid/hub-frontend:latest \
  -f myIoTGrid.Hub/myIoTGrid.Hub.Frontend/docker/Dockerfile \
  myIoTGrid.Hub/myIoTGrid.Hub.Frontend

# Sensor Simulator
docker build -t myiotgrid/sensor-sim:latest \
  -f myIoTGrid.Sensor/docker/Dockerfile \
  myIoTGrid.Sensor
```

## Stack verwalten

### Starten

```bash
# Alle Services starten
docker compose up -d

# Mit Live-Logs
docker compose up

# Einzelnen Service starten
docker compose up -d hub-api
```

### Stoppen

```bash
# Alle Services stoppen
docker compose down

# Stoppen und Volumes löschen (Achtung: Datenverlust!)
docker compose down -v
```

### Logs anzeigen

```bash
# Alle Logs
docker compose logs

# Live-Logs
docker compose logs -f

# Logs eines Services
docker compose logs hub-api
docker compose logs hub-frontend
docker compose logs sensor-sim
docker compose logs mosquitto
```

### Status prüfen

```bash
# Container-Status
docker compose ps

# Health-Checks
curl -k https://localhost:5001/health
curl -k https://localhost/health
```

## Datenpersistenz

Die Container verwenden Docker Volumes für persistente Daten:

| Volume | Inhalt |
|--------|--------|
| `myiotgrid-hub-data` | SQLite Datenbank |
| `myiotgrid-hub-logs` | Backend Logs |
| `myiotgrid-hub-certs` | SSL Zertifikate (Backend) |
| `myiotgrid-mosquitto-data` | MQTT Daten |
| `myiotgrid-mosquitto-logs` | MQTT Logs |
| `myiotgrid-sensor-data` | Sensor Konfiguration |

```bash
# Volumes anzeigen
docker volume ls | grep myiotgrid

# Volume inspizieren
docker volume inspect myiotgrid-hub-data
```

## Konfiguration

### Umgebungsvariablen

Die wichtigsten Umgebungsvariablen können in der `docker-compose.yml` oder einer `.env` Datei gesetzt werden:

**Backend (hub-api):**
```env
ASPNETCORE_ENVIRONMENT=Production
ConnectionStrings__HubDb=Data Source=/app/data/hub.db
Mqtt__Host=mosquitto
Mqtt__Port=1883
```

**Sensor Simulator:**
```env
HUB_HOST=hub-api
HUB_PORT=5001
HUB_PROTOCOL=https
HUB_INSECURE=true
SENSOR_ID=sim-sensor-01
```

### Eigene SSL-Zertifikate

Für Production solltest du eigene SSL-Zertifikate verwenden:

```bash
# Zertifikate-Verzeichnis erstellen
mkdir -p certs

# Zertifikate kopieren
cp /path/to/fullchain.pem certs/
cp /path/to/privkey.pem certs/

# Stack neu starten
docker compose down
docker compose up -d
```

Das Frontend mountet automatisch `./certs:/etc/nginx/certs:ro`.

## Troubleshooting

### Container startet nicht

```bash
# Logs prüfen
docker compose logs hub-api

# Container manuell starten für Debug
docker compose run --rm hub-api sh
```

### Port bereits belegt

```bash
# Prüfen welcher Prozess den Port nutzt
lsof -i :5001
lsof -i :443

# Ports in docker-compose.yml ändern
ports:
  - "5002:5001"  # Externer Port ändern
```

### Datenbank zurücksetzen

```bash
# Achtung: Alle Daten werden gelöscht!
docker compose down -v
docker compose up -d
```

### Rebuild nach Code-Änderungen

```bash
# Service neu bauen und starten
docker compose up -d --build hub-api

# Oder alle Services
docker compose up -d --build
```

## Projektstruktur

```
myIoTGrid/
├── docker-compose.yml              # Zentrale Docker Compose Konfiguration
├── build-docker.sh                 # Build-Script
├── DOCKER.md                       # Diese Dokumentation
├── docker/
│   └── mosquitto/
│       └── mosquitto.conf          # MQTT Broker Konfiguration
├── myIoTGrid.Hub/
│   ├── myIoTGrid.Hub.Backend/
│   │   └── src/myIoTGrid.Hub.Api/
│   │       └── Dockerfile          # Backend Dockerfile
│   └── myIoTGrid.Hub.Frontend/
│       └── docker/
│           ├── Dockerfile          # Frontend Dockerfile
│           └── nginx.conf          # nginx HTTPS Konfiguration
└── myIoTGrid.Sensor/
    └── docker/
        └── Dockerfile              # Sensor Simulator Dockerfile
```

## CI/CD

GitHub Actions bauen automatisch alle Docker Images bei:
- Push auf `main` oder `develop`
- Tags mit `v*` (z.B. `v1.0.0`)
- Pull Requests

Images werden in der GitHub Container Registry veröffentlicht:
- `ghcr.io/frankseins/myiotgrid/hub-api:latest`
- `ghcr.io/frankseins/myiotgrid/hub-frontend:latest`
- `ghcr.io/frankseins/myiotgrid/sensor-sim:latest`

## Raspberry Pi Deployment

Für Raspberry Pi (ARM64) werden automatisch Multi-Arch Images gebaut:

```bash
# Auf Raspberry Pi
docker compose pull
docker compose up -d
```
