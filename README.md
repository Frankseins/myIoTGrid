<h1 align="center">myIoTGrid</h1>

<p align="center">
  <strong>Dein persönliches Sensornetz. Von Smart Home zu Smart World. Powered by AI.</strong>
</p>

<p align="center">
  <a href="https://myiotgrid.cloud">🌐 Website</a> •
  <a href="https://mysocialcare-doku.atlassian.net/wiki/spaces/myIoTGrid">📚 Dokumentation</a> •
  <a href="#-schnellstart">🚀 Schnellstart</a> •
  <a href="#-community">💬 Community</a>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/License-MIT-green.svg" alt="License: MIT"/>
  <img src="https://img.shields.io/badge/Status-Development-orange.svg" alt="Status: Development"/>
  <img src="https://img.shields.io/badge/.NET-8.0%20%7C%2010.0-purple.svg" alt=".NET"/>
  <img src="https://img.shields.io/badge/Angular-18+-red.svg" alt="Angular"/>
  <img src="https://img.shields.io/badge/AI-Native-blue.svg" alt="AI Native"/>
</p>

---

## 🌟 Was ist myIoTGrid?

**myIoTGrid** ist eine Open-Source-Plattform zur Erfassung, Visualisierung und gemeinsamen Nutzung von Sensordaten. Das System ermöglicht es Privatpersonen, Schulen, Kommunen und Unternehmen, eigene Sensornetzwerke aufzubauen – von der einzelnen Wetterstation bis zum stadtweiten Umweltmonitoring.

### ✨ Highlights

- 🏠 **Local First** – Funktioniert 100% offline, keine Cloud erforderlich
- 🤖 **AI Native** – KI-gestützte Analysen, Warnungen und Prognosen
- 🔐 **Privacy by Design** – Du entscheidest, wer deine Daten sieht
- 🏘️ **Community Sharing** – Teile Daten mit Nachbarn, der Stadt oder der Welt
- 🏡 **Smart Home Ready** – Apple Home, Google Home, Alexa via Matter
- 💰 **Open Source** – MIT-Lizenz, keine versteckten Kosten

---

## 🤖 Built with AI – Powered by AI

myIoTGrid ist von Anfang an ein **KI-natives Projekt**:

```
┌─────────────────────────────────────────────────────────────────┐
│  🤖 KI-INSIGHTS                                   myiotgrid.cloud │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│  ⚠️ WARNUNGEN                                                   │
│  ├─ 🔴 Keller: Schimmelrisiko in 3 Tagen (Feuchte + Temp)      │
│  ├─ 🟡 Garten: Frostgefahr heute Nacht (Prognose: -2°C)        │
│  └─ 🟢 Wohnzimmer: Optimales Raumklima ✓                       │
│                                                                  │
│  📈 ERKENNTNISSE                                                │
│  ├─ "Deine Luftqualität ist morgens 40% schlechter als abends" │
│  └─ "Dein Keller ist feuchter als 80% der Community"           │
│                                                                  │
│  🔮 PROGNOSEN                                                   │
│  ├─ Temperatur morgen: 12°C → 18°C                             │
│  └─ Feinstaub-Trend: ↗️ Anstieg erwartet                       │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

| KI-Feature | Beschreibung |
|------------|--------------|
| **🔍 Anomalie-Erkennung** | Lernt normales Verhalten, warnt bei Abweichungen |
| **⚠️ Prädiktive Warnungen** | Frost, Schimmel, Hochwasser – Stunden vorher |
| **🧠 Community Intelligence** | Je mehr Nutzer, desto schlauer die KI |
| **💡 Persönliche Empfehlungen** | Individuelle Tipps für deine Situation |

**KI-Warnstufen:**

| 🔴 Kritisch | 🟡 Warnung | 🔵 Hinweis | 🟢 OK |
|-------------|------------|------------|-------|
| Sofort handeln | Aufmerksamkeit | Optimierung | Alles gut |

---

## 🏗️ Architektur

myIoTGrid besteht aus drei Komponenten:

```
┌─────────────────┐     ┌─────────────────┐     ┌─────────────────┐
│  Grid.Sensor    │────▶│    Grid.Hub     │────▶│   Grid.Cloud    │
│  (ESP32)        │MQTT │  (Raspberry Pi) │HTTPS│   (Optional)    │
│                 │     │                 │     │                 │
│  🌡️ Sensoren    │     │  🧠 Edge-KI     │     │  🤖 Cloud-KI    │
│  ~10-15€        │     │  📊 Dashboard   │     │  🗺️ Community   │
│                 │     │  🏠 Matter      │     │  🤝 Sharing     │
└─────────────────┘     └─────────────────┘     └─────────────────┘
```

### Grid.Sensor – Die Augen 👁️

| Eigenschaft | Spezifikation |
|-------------|---------------|
| **Hardware** | ESP32 NodeMCU (~5€), ESP32-C3, ESP8266 |
| **Protokoll** | MQTT, optional HTTPS |
| **Sensoren** | Temperatur, Feuchte, CO2, Feinstaub, Licht, Boden, Wind |
| **Kosten** | Ab ~10-15€ pro Sensor |

### Grid.Hub – Das Gehirn 🧠

| Eigenschaft | Spezifikation |
|-------------|---------------|
| **Hardware** | Raspberry Pi 4/5, alter PC, Docker |
| **Backend** | .NET 8 (ASP.NET Core) |
| **Frontend** | Angular 18+ (PWA) |
| **Datenbank** | SQLite (kein Server nötig) |
| **Smart Home** | Matter.js → Apple Home, Google, Alexa |
| **KI** | ML.NET / ONNX Runtime (offline!) |

### Grid.Cloud – Das Netzwerk 🌐 (Optional)

| Eigenschaft | Spezifikation |
|-------------|---------------|
| **Backend** | .NET 10 (ASP.NET Core) |
| **Datenbank** | PostgreSQL (Multi-Tenant) |
| **Features** | Community Map, Sharing, Subscriptions |
| **KI** | Cloud-KI, Community Intelligence |

---

## 🔐 Sharing-Modell

**Jeder Sensor ist standardmäßig PRIVAT.** Du entscheidest:

| Stufe | Symbol | Wer sieht's? | KI-Vorteil |
|-------|--------|--------------|------------|
| **Privat** | 🔒 | Nur du | Nur deine Daten |
| **Geteilt** | 👥 | Ausgewählte Personen | Gruppen-Insights |
| **Community** | 🏘️ | Alle myIoTGrid-Nutzer | KI lernt von allen |
| **Öffentlich** | 🌍 | Jeder (Open Data) | Wissenschaft |

---

## 📡 Unterstützte Sensoren

| Kategorie | Sensoren | Messgrößen |
|-----------|----------|------------|
| **Temperatur** | DHT22, BME280, BME680, DS18B20, SHT31 | °C |
| **Luftfeuchte** | DHT22, BME280, BME680, SHT31 | % |
| **Luftdruck** | BME280, BME680, BMP280 | hPa |
| **CO2** | MH-Z19B, SCD30, SCD40 | ppm |
| **Feinstaub** | SDS011, PMS5003, SPS30 | µg/m³ |
| **VOC** | BME680, SGP30, SGP40 | ppb |
| **Licht** | BH1750, TSL2561 | Lux |
| **Bodenfeuchte** | Capacitive Soil Sensor | % |
| **Wind** | Anemometer | km/h |

---

## 🌐 Externe Datenquellen

myIoTGrid integriert automatisch:

| Quelle | Daten | Sensoren |
|--------|-------|----------|
| **Sensor.Community** | Feinstaub, Temperatur | 34.000+ weltweit |
| **OpenWeatherMap** | Wetter, Vorhersagen | Global |
| **DWD Open Data** | Wetterwarnungen | 2.000+ (Deutschland) |
| **OpenAQ** | Luftqualität | 10.000+ weltweit |

---

## 🚀 Schnellstart

### Voraussetzungen

- Raspberry Pi 4/5 (oder Docker auf beliebigem System)
- ESP32 NodeMCU + Sensor (z.B. BME280)
- WiFi-Netzwerk

### 1. Grid.Hub installieren

```bash
# Option A: Docker (empfohlen)
docker run -d \
  --name myiotgrid-hub \
  -p 5000:5000 \
  -p 1883:1883 \
  -v myiotgrid-data:/app/data \
  ghcr.io/myiotgrid/hub:latest

# Option B: Direkt auf Raspberry Pi
curl -sSL https://get.myiotgrid.cloud | bash
```

### 2. Grid.Sensor flashen

```bash
# PlatformIO
cd grid-sensor
cp config.example.h config.h
# WiFi und Hub-IP in config.h eintragen
pio run --target upload
```

### 3. Dashboard öffnen

```
http://raspberry-pi-ip:5000
```

---

## 📁 Projektstruktur

```
myIoTGrid/
├── grid-sensor/              # ESP32 Firmware (C++/PlatformIO)
│   ├── src/
│   ├── lib/
│   └── platformio.ini
│
├── grid-hub/                 # Lokaler Hub
│   ├── src/
│   │   ├── Grid.Hub.Api/           # .NET 8 Backend
│   │   ├── Grid.Hub.Domain/        # Entities
│   │   ├── Grid.Hub.Infrastructure/# EF Core, SQLite
│   │   ├── Grid.Hub.Service/       # Business Logic
│   │   └── Grid.Hub.Shared/        # DTOs
│   ├── web/                        # Angular Frontend
│   └── matter/                     # Matter.js Bridge
│
├── grid-cloud/               # Cloud Platform (optional)
│   ├── src/
│   │   ├── Grid.Cloud.Api/         # .NET 10 Backend
│   │   └── ...
│   └── web/                        # Angular Frontend
│
├── docs/                     # Dokumentation
└── docker/                   # Docker Compose Files
```

---

## 🎯 Zielgruppen

| Zielgruppe | Use Case | KI-Benefit |
|------------|----------|------------|
| **🔧 Maker & DIY** | Eigene Sensoren bauen | KI-APIs zum Experimentieren |
| **🏠 Smart-Home-Nutzer** | Apple/Google/Alexa Integration | Proaktive Warnungen |
| **👨‍👩‍👧‍👦 Familien** | Ferienhaus, Oma's Haus überwachen | KI für alle Standorte |
| **🏫 Schulen** | MINT-Projekte | KI-Projekte mit echten Daten |
| **🌾 Landwirte** | Felder, Gewächshäuser | Bewässerungs- & Frost-Vorhersage |
| **🏙️ Kommunen** | Stadtweites Monitoring | Hitze-Karten, Frühwarnung |
| **🔬 Forscher** | Bürgerwissenschaft | KI-Modelle auf Community-Daten |

---

## 🛠️ Technologie-Stack

| Komponente | Technologie |
|------------|-------------|
| **Sensor Firmware** | C++, PlatformIO, Arduino |
| **Hub Backend** | .NET 8, ASP.NET Core, SignalR |
| **Hub Frontend** | Angular 18+, TypeScript, Tailwind |
| **Hub Datenbank** | SQLite |
| **Hub KI** | ML.NET, ONNX Runtime |
| **Hub Smart Home** | Matter.js (Node.js) |
| **Cloud Backend** | .NET 10, ASP.NET Core |
| **Cloud Datenbank** | PostgreSQL |
| **Cloud KI** | ML.NET, Python ML Services |
| **Container** | Docker, Docker Compose |

---

## 📜 Leitprinzipien

| Prinzip | Umsetzung |
|---------|-----------|
| **Local First** | Hub funktioniert 100% offline |
| **Privacy by Design** | Sensoren sind standardmäßig privat |
| **Open Source** | MIT-Lizenz, keine Einschränkungen |
| **Community Driven** | Entwicklung durch die Community |
| **AI Native** | KI von Tag 1 integriert |

---

## 🗺️ Roadmap

| Phase | Beschreibung | Status |
|-------|--------------|--------|
| **1** | Hub Standalone + Matter MVP | 🎯 Hackathon |
| **2** | Sensor Plugin-System | 📋 Geplant |
| **3** | Grid.Cloud MVP | 📋 Geplant |
| **4** | Community (Sharing, Map) | 📋 Geplant |
| **5** | Externe Quellen | 📋 Geplant |
| **6** | Prädiktive Warnungen | 📋 Geplant |
| **7** | Smart Home Cloud APIs | 📋 Geplant |
| **8** | Mobile App | 📋 Geplant |

---

## 🤝 Mitmachen

Wir freuen uns über Beiträge! 

### So kannst du helfen:

- 🐛 **Bug Reports** – Issues erstellen
- 💡 **Feature Requests** – Ideen einbringen
- 🔧 **Code** – Pull Requests willkommen
- 📚 **Dokumentation** – Anleitungen verbessern
- 🌍 **Übersetzungen** – Sprachen hinzufügen
- 🤖 **KI/ML** – Modelle entwickeln

### Development Setup

```bash
# Repository klonen
git clone https://github.com/myiotgrid/myiotgrid.git
cd myiotgrid

# Hub Backend
cd grid-hub/src
dotnet restore
dotnet build

# Hub Frontend
cd ../web
npm install
ng serve

# Sensor Firmware
cd ../../grid-sensor
pio run
```

---

## 📄 Lizenz

MIT License – **Keine Einschränkungen. Für immer frei.**

```
MIT License

Copyright (c) 2025 myIoTGrid Contributors

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software...
```

**Warum MIT?**
- Weil Umweltdaten allen gehören sollten
- Weil KI für alle da sein sollte – nicht nur für Big Tech
- Weil eine Community mehr erreicht als ein Unternehmen

---

## 💬 Community

- 🌐 **Website:** [myiotgrid.cloud](https://myiotgrid.cloud)
- 📚 **Docs:** [Confluence](https://mysocialcare-doku.atlassian.net/wiki/spaces/myIoTGrid)
- 💬 **Discord:** [Bald verfügbar]
- 🐦 **Twitter:** [@myiotgrid](https://twitter.com/myiotgrid)

---

## 🙏 Danksagungen

- [Sensor.Community](https://sensor.community) – Inspiration & Datenquelle
- [Matter.js](https://github.com/project-chip/matter.js) – Smart Home Bridge
- [Anthropic Claude](https://claude.ai) – KI-unterstützte Entwicklung

---

<p align="center">
  <strong>myIoTGrid – Dein persönliches Sensornetz.</strong><br/>
  <em>Open Source. Community Driven. Privacy First. AI Native.</em>
</p>

<p align="center">
  ⭐ Star uns auf GitHub, wenn dir das Projekt gefällt!
</p>
