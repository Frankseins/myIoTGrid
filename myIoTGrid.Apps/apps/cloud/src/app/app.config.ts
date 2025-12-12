import {
  ApplicationConfig,
  provideBrowserGlobalErrorListeners,
  provideZoneChangeDetection,
} from '@angular/core';
import { provideRouter, withComponentInputBinding } from '@angular/router';
import { provideHttpClient, withFetch } from '@angular/common/http';
import { provideAnimationsAsync } from '@angular/platform-browser/animations/async';
import { appRoutes } from './app.routes';
import { API_CONFIG, ApiConfig } from '@myiotgrid/shared/data-access';

/**
 * API Configuration for Cloud Frontend
 *
 * Cloud API runs on port 5002 (Hub API runs on 5001)
 *
 * For PRODUCTION:
 * - Use window.location.origin (API and frontend on same host)
 */
const apiConfig: ApiConfig = {
  baseUrl: '/api',
  signalRUrl: '/hubs/sensors',
  // Cloud API on port 5002
  sensorApiUrl: typeof window !== 'undefined'
    ? (window.location.hostname === 'localhost'
        ? 'https://localhost:5002'
        : window.location.origin)
    : 'https://localhost:5002'
};

export const appConfig: ApplicationConfig = {
  providers: [
    provideBrowserGlobalErrorListeners(),
    provideZoneChangeDetection({ eventCoalescing: true }),
    provideRouter(appRoutes, withComponentInputBinding()),
    provideHttpClient(withFetch()),
    provideAnimationsAsync(),
    { provide: API_CONFIG, useValue: apiConfig }
  ],
};
