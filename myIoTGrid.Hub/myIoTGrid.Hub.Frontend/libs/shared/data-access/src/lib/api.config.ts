import { InjectionToken } from '@angular/core';

export interface ApiConfig {
  baseUrl: string;
  signalRUrl: string;
}

export const API_CONFIG = new InjectionToken<ApiConfig>('API_CONFIG');

export const defaultApiConfig: ApiConfig = {
  baseUrl: '/api',
  signalRUrl: '/hubs/sensors'
};
