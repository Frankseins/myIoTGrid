import {
  Component,
  Input,
  OnChanges,
  OnDestroy,
  OnInit,
  SimpleChanges,
  ViewChild,
  ElementRef,
  AfterViewInit
} from '@angular/core';
import { CommonModule } from '@angular/common';
import { Chart, registerables, TimeUnit } from 'chart.js';
import 'chartjs-adapter-date-fns';
import { format } from 'date-fns';
import { de } from 'date-fns/locale';
import { ChartPoint, ChartInterval } from '@myiotgrid/shared/models';

// Register all Chart.js components
Chart.register(...registerables);

@Component({
  selector: 'myiotgrid-line-chart',
  standalone: true,
  imports: [CommonModule],
  templateUrl: './line-chart.component.html',
  styleUrl: './line-chart.component.scss'
})
export class LineChartComponent implements OnInit, AfterViewInit, OnChanges, OnDestroy {
  @ViewChild('chartCanvas') chartCanvas!: ElementRef<HTMLCanvasElement>;

  @Input() dataPoints: ChartPoint[] = [];
  @Input() color = '#FF5722';
  @Input() unit = '';
  @Input() showMinMax = true;
  @Input() height = 300;
  @Input() interval: ChartInterval = ChartInterval.OneDay;

  private chart: Chart | null = null;

  ngOnInit(): void {
    // Chart will be initialized after view init
  }

  ngAfterViewInit(): void {
    this.createChart();
  }

  ngOnChanges(changes: SimpleChanges): void {
    if ((changes['dataPoints'] || changes['color'] || changes['interval']) && this.chart) {
      // Bei Interval-Änderung Chart neu erstellen für korrekte X-Achse
      if (changes['interval']) {
        this.chart.destroy();
        this.chart = null;
        this.createChart();
      } else {
        this.updateChart();
      }
    }
  }

  ngOnDestroy(): void {
    if (this.chart) {
      this.chart.destroy();
      this.chart = null;
    }
  }

  private createChart(): void {
    if (!this.chartCanvas?.nativeElement) return;

    const ctx = this.chartCanvas.nativeElement.getContext('2d');
    if (!ctx) return;

    // Create gradient for fill
    const gradient = ctx.createLinearGradient(0, 0, 0, this.height);
    gradient.addColorStop(0, this.hexToRgba(this.color, 0.4));
    gradient.addColorStop(1, this.hexToRgba(this.color, 0.05));

    // Determine point size based on data count - always show small points for better interaction
    const pointRadius = this.dataPoints.length > 100 ? 2 : 4;

    const datasets: any[] = [
      {
        label: 'Wert',
        data: this.dataPoints.map(p => ({
          x: new Date(p.timestamp),
          y: p.value
        })),
        borderColor: this.color,
        backgroundColor: gradient,
        fill: true,
        tension: 0.3,
        borderWidth: 2,
        pointRadius: pointRadius,
        pointBackgroundColor: this.color,
        pointBorderColor: '#fff',
        pointBorderWidth: 1,
        pointHitRadius: 20,
        pointHoverRadius: 8,
        pointHoverBackgroundColor: this.color,
        pointHoverBorderColor: '#fff',
        pointHoverBorderWidth: 2
      }
    ];

    // Add min/max bands if enabled and data available
    if (this.showMinMax && this.dataPoints.some(p => p.min !== null && p.max !== null)) {
      datasets.push({
        label: 'Max',
        data: this.dataPoints.map(p => ({
          x: new Date(p.timestamp),
          y: p.max ?? p.value
        })),
        borderColor: this.hexToRgba(this.color, 0.3),
        backgroundColor: 'transparent',
        fill: false,
        tension: 0.3,
        borderWidth: 1,
        borderDash: [5, 5],
        pointRadius: 0
      });

      datasets.push({
        label: 'Min',
        data: this.dataPoints.map(p => ({
          x: new Date(p.timestamp),
          y: p.min ?? p.value
        })),
        borderColor: this.hexToRgba(this.color, 0.3),
        backgroundColor: 'transparent',
        fill: false,
        tension: 0.3,
        borderWidth: 1,
        borderDash: [5, 5],
        pointRadius: 0
      });
    }

    // Get time unit and format based on interval
    const { timeUnit, tooltipFormat, displayFormat } = this.getTimeConfig();

    // Store references for callbacks
    const unit = this.unit;
    const currentTooltipFormat = tooltipFormat;

    this.chart = new Chart(ctx, {
      type: 'line',
      data: { datasets },
      options: {
        responsive: true,
        maintainAspectRatio: false,
        events: ['mousemove', 'mouseout', 'click', 'touchstart', 'touchmove'],
        hover: {
          mode: 'index',
          intersect: false
        },
        interaction: {
          mode: 'index',
          intersect: false,
          axis: 'x'
        },
        plugins: {
          legend: {
            display: false
          },
          tooltip: {
            enabled: true,
            mode: 'index',
            intersect: false,
            position: 'nearest',
            backgroundColor: 'rgba(33, 33, 33, 0.95)',
            titleColor: '#fff',
            bodyColor: '#fff',
            padding: 16,
            cornerRadius: 8,
            displayColors: false,
            titleFont: {
              size: 13,
              weight: 'normal'
            },
            bodyFont: {
              size: 18,
              weight: 'bold'
            },
            filter: (tooltipItem) => {
              // Only show tooltip for main dataset (not min/max lines)
              return tooltipItem.datasetIndex === 0;
            },
            callbacks: {
              title: (items) => {
                if (items.length > 0) {
                  // parsed.x is a timestamp in milliseconds, convert to Date
                  const timestamp = items[0].parsed.x;
                  const dateObj = new Date(timestamp);
                  // Use date-fns format directly instead of Angular DatePipe
                  return format(dateObj, currentTooltipFormat, { locale: de });
                }
                return '';
              },
              label: (item) => {
                if (item.datasetIndex === 0) {
                  return `${item.parsed.y.toFixed(1)} ${unit}`;
                }
                return '';
              }
            }
          }
        },
        scales: {
          x: {
            type: 'time',
            adapters: {
              date: {
                locale: de
              }
            },
            time: {
              unit: timeUnit,
              displayFormats: {
                minute: 'HH:mm',
                hour: 'HH:mm',
                day: 'dd. MMM',
                week: 'dd. MMM',
                month: 'MMM yyyy'
              },
              tooltipFormat: tooltipFormat
            },
            grid: {
              display: true,
              color: 'rgba(0, 0, 0, 0.06)'
            },
            ticks: {
              color: '#666',
              maxRotation: 0,
              autoSkip: true,
              maxTicksLimit: 8,
              font: {
                size: 11
              }
            }
          },
          y: {
            grid: {
              display: true,
              color: 'rgba(0, 0, 0, 0.06)'
            },
            ticks: {
              color: '#666',
              font: {
                size: 11
              },
              callback: (value) => `${value} ${this.unit}`
            }
          }
        }
      }
    });
  }

  private getTimeConfig(): { timeUnit: TimeUnit; tooltipFormat: string; displayFormat: string } {
    switch (this.interval) {
      case ChartInterval.OneHour:
        return { timeUnit: 'minute', tooltipFormat: 'dd.MM.yyyy HH:mm', displayFormat: 'HH:mm' };
      case ChartInterval.OneDay:
        return { timeUnit: 'hour', tooltipFormat: 'dd.MM.yyyy HH:mm', displayFormat: 'HH:mm' };
      case ChartInterval.OneWeek:
        return { timeUnit: 'day', tooltipFormat: 'dd.MM.yyyy HH:mm', displayFormat: 'dd. MMM' };
      case ChartInterval.OneMonth:
        return { timeUnit: 'day', tooltipFormat: 'dd.MM.yyyy', displayFormat: 'dd. MMM' };
      case ChartInterval.ThreeMonths:
        return { timeUnit: 'week', tooltipFormat: 'dd.MM.yyyy', displayFormat: 'dd. MMM' };
      case ChartInterval.SixMonths:
        return { timeUnit: 'week', tooltipFormat: 'dd.MM.yyyy', displayFormat: 'dd. MMM' };
      case ChartInterval.OneYear:
        return { timeUnit: 'month', tooltipFormat: 'MMM yyyy', displayFormat: 'MMM yyyy' };
      default:
        return { timeUnit: 'hour', tooltipFormat: 'dd.MM.yyyy HH:mm', displayFormat: 'HH:mm' };
    }
  }

  private updateChart(): void {
    if (!this.chart) {
      this.createChart();
      return;
    }

    const ctx = this.chartCanvas?.nativeElement?.getContext('2d');
    if (!ctx) return;

    // Update gradient
    const gradient = ctx.createLinearGradient(0, 0, 0, this.height);
    gradient.addColorStop(0, this.hexToRgba(this.color, 0.4));
    gradient.addColorStop(1, this.hexToRgba(this.color, 0.05));

    // Update main dataset - cast as any for Chart.js time scale compatibility
    this.chart.data.datasets[0].data = this.dataPoints.map(p => ({
      x: new Date(p.timestamp),
      y: p.value
    })) as any;
    this.chart.data.datasets[0].borderColor = this.color;
    this.chart.data.datasets[0].backgroundColor = gradient;
    (this.chart.data.datasets[0] as any).pointHoverBackgroundColor = this.color;

    // Update min/max if present
    if (this.chart.data.datasets.length > 1) {
      this.chart.data.datasets[1].data = this.dataPoints.map(p => ({
        x: new Date(p.timestamp),
        y: p.max ?? p.value
      })) as any;
      this.chart.data.datasets[1].borderColor = this.hexToRgba(this.color, 0.3);
    }

    if (this.chart.data.datasets.length > 2) {
      this.chart.data.datasets[2].data = this.dataPoints.map(p => ({
        x: new Date(p.timestamp),
        y: p.min ?? p.value
      })) as any;
      this.chart.data.datasets[2].borderColor = this.hexToRgba(this.color, 0.3);
    }

    this.chart.update('none');
  }

  private hexToRgba(hex: string, alpha: number): string {
    const result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
    if (!result) return `rgba(0, 0, 0, ${alpha})`;

    const r = parseInt(result[1], 16);
    const g = parseInt(result[2], 16);
    const b = parseInt(result[3], 16);

    return `rgba(${r}, ${g}, ${b}, ${alpha})`;
  }
}
