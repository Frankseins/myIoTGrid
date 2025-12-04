#!/bin/bash
# =============================================================================
# myIoTGrid - Docker Build Script
# =============================================================================
# Builds all 4 Docker images locally
# Usage: ./build-docker.sh [--push] [--no-cache]
# =============================================================================

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
REGISTRY="ghcr.io"
IMAGE_PREFIX="frankseins/myiotgrid"
TAG="${TAG:-latest}"

# Parse arguments
PUSH=false
NO_CACHE=""
while [[ "$#" -gt 0 ]]; do
    case $1 in
        --push) PUSH=true ;;
        --no-cache) NO_CACHE="--no-cache" ;;
        --tag) TAG="$2"; shift ;;
        *) echo "Unknown parameter: $1"; exit 1 ;;
    esac
    shift
done

echo -e "${BLUE}=============================================${NC}"
echo -e "${BLUE}  myIoTGrid Docker Build${NC}"
echo -e "${BLUE}=============================================${NC}"
echo ""
echo -e "Registry: ${YELLOW}${REGISTRY}${NC}"
echo -e "Prefix:   ${YELLOW}${IMAGE_PREFIX}${NC}"
echo -e "Tag:      ${YELLOW}${TAG}${NC}"
echo -e "Push:     ${YELLOW}${PUSH}${NC}"
echo ""

# Function to build an image
build_image() {
    local name=$1
    local context=$2
    local dockerfile=$3
    local image="${REGISTRY}/${IMAGE_PREFIX}/${name}:${TAG}"

    echo -e "${BLUE}---------------------------------------------${NC}"
    echo -e "${BLUE}Building: ${name}${NC}"
    echo -e "${BLUE}---------------------------------------------${NC}"
    echo -e "Image:      ${YELLOW}${image}${NC}"
    echo -e "Context:    ${YELLOW}${context}${NC}"
    echo -e "Dockerfile: ${YELLOW}${dockerfile}${NC}"
    echo ""

    if docker build ${NO_CACHE} -t "${image}" -f "${dockerfile}" "${context}"; then
        echo -e "${GREEN}✓ ${name} built successfully${NC}"

        if [ "$PUSH" = true ]; then
            echo -e "${BLUE}Pushing ${name}...${NC}"
            docker push "${image}"
            echo -e "${GREEN}✓ ${name} pushed successfully${NC}"
        fi
    else
        echo -e "${RED}✗ ${name} build failed${NC}"
        exit 1
    fi
    echo ""
}

# Change to project root directory
cd "$(dirname "$0")"

# Build all images
echo -e "${YELLOW}Building all Docker images...${NC}"
echo ""

# 1. Hub API
build_image "hub-api" \
    "./myIoTGrid.Hub/myIoTGrid.Hub.Backend" \
    "./myIoTGrid.Hub/myIoTGrid.Hub.Backend/src/myIoTGrid.Hub.Api/Dockerfile"

# 2. Hub Frontend
build_image "hub-frontend" \
    "./myIoTGrid.Hub/myIoTGrid.Hub.Frontend" \
    "./myIoTGrid.Hub/myIoTGrid.Hub.Frontend/docker/Dockerfile"

# 3. Sensor Simulator
build_image "sensor-sim" \
    "./myIoTGrid.Sensor" \
    "./myIoTGrid.Sensor/docker/Dockerfile"

# Summary
echo -e "${BLUE}=============================================${NC}"
echo -e "${GREEN}  Build Complete!${NC}"
echo -e "${BLUE}=============================================${NC}"
echo ""
echo -e "Images built:"
echo -e "  • ${REGISTRY}/${IMAGE_PREFIX}/hub-api:${TAG}"
echo -e "  • ${REGISTRY}/${IMAGE_PREFIX}/hub-frontend:${TAG}"
echo -e "  • ${REGISTRY}/${IMAGE_PREFIX}/sensor-sim:${TAG}"
echo ""
echo -e "To start the stack:"
echo -e "  ${YELLOW}docker compose up -d${NC}"
echo ""
echo -e "Ports (all HTTPS):"
echo -e "  • Backend API:  ${YELLOW}https://localhost:5001${NC}"
echo -e "  • Frontend:     ${YELLOW}https://localhost${NC} (Port 443)"
echo -e "  • MQTT:         ${YELLOW}localhost:1883${NC}"
echo ""
echo -e "Note: Self-signed certificates are generated automatically."
echo -e "      Browsers will show a security warning - this is expected."
echo ""
