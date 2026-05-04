// stats.js - Statistics and Charts

class StatsDashboard {
    constructor() {
        this.charts = {};
        this.data = {};
        this.isInitialized = false;
    }

    init() {
        if (this.isInitialized) return;

        this.createCharts();
        this.loadData();
        this.isInitialized = true;

        // Auto-refresh every 30 seconds
        setInterval(() => {
            this.loadData();
        }, 30000);
    }

    async loadData() {
        try {
            const response = await fetch('/api/stats');
            if (!response.ok) {
                throw new Error('Failed to load stats data');
            }
            this.data = await response.json();
            this.updateCharts();
            this.updateStatsCards();
        } catch (error) {
            console.error('Error loading stats data:', error);
            this.showError('Failed to load statistics data');
        }
    }

    createCharts() {
        // Score Distribution Chart
        this.charts.scoreDistribution = new Chart(
            document.getElementById('score-distribution-chart'),
            {
                type: 'bar',
                data: {
                    labels: [],
                    datasets: [{
                        label: 'Score Distribution',
                        data: [],
                        backgroundColor: 'rgba(0, 212, 255, 0.6)',
                        borderColor: '#00d4ff',
                        borderWidth: 1
                    }]
                },
                options: {
                    responsive: true,
                    maintainAspectRatio: false,
                    plugins: {
                        legend: {
                            labels: {
                                color: '#e2e8f0'
                            }
                        }
                    },
                    scales: {
                        y: {
                            beginAtZero: true,
                            grid: {
                                color: 'rgba(255, 255, 255, 0.1)'
                            },
                            ticks: {
                                color: '#94a3b8'
                            }
                        },
                        x: {
                            grid: {
                                color: 'rgba(255, 255, 255, 0.1)'
                            },
                            ticks: {
                                color: '#94a3b8'
                            }
                        }
                    }
                }
            }
        );

        // Performance Trends Chart
        this.charts.performanceTrends = new Chart(
            document.getElementById('performance-trends-chart'),
            {
                type: 'line',
                data: {
                    labels: [],
                    datasets: [{
                        label: 'Average Score',
                        data: [],
                        borderColor: '#00d4ff',
                        backgroundColor: 'rgba(0, 212, 255, 0.1)',
                        tension: 0.4,
                        fill: true
                    }]
                },
                options: {
                    responsive: true,
                    maintainAspectRatio: false,
                    plugins: {
                        legend: {
                            labels: {
                                color: '#e2e8f0'
                            }
                        }
                    },
                    scales: {
                        y: {
                            beginAtZero: true,
                            grid: {
                                color: 'rgba(255, 255, 255, 0.1)'
                            },
                            ticks: {
                                color: '#94a3b8'
                            }
                        },
                        x: {
                            grid: {
                                color: 'rgba(255, 255, 255, 0.1)'
                            },
                            ticks: {
                                color: '#94a3b8'
                            }
                        }
                    }
                }
            }
        );

        // Rank Distribution Chart
        this.charts.rankDistribution = new Chart(
            document.getElementById('rank-distribution-chart'),
            {
                type: 'doughnut',
                data: {
                    labels: ['Top 10%', 'Top 25%', 'Top 50%', 'Bottom 50%'],
                    datasets: [{
                        data: [],
                        backgroundColor: [
                            '#10b981',
                            '#00d4ff',
                            '#f59e0b',
                            '#ef4444'
                        ],
                        borderWidth: 2,
                        borderColor: 'rgba(15, 23, 42, 0.8)'
                    }]
                },
                options: {
                    responsive: true,
                    maintainAspectRatio: false,
                    plugins: {
                        legend: {
                            labels: {
                                color: '#e2e8f0'
                            }
                        }
                    }
                }
            }
        );

        // Activity Timeline Chart
        this.charts.activityTimeline = new Chart(
            document.getElementById('activity-timeline-chart'),
            {
                type: 'line',
                data: {
                    labels: [],
                    datasets: [{
                        label: 'Activity Events',
                        data: [],
                        borderColor: '#10b981',
                        backgroundColor: 'rgba(16, 185, 129, 0.1)',
                        tension: 0.4,
                        fill: true
                    }]
                },
                options: {
                    responsive: true,
                    maintainAspectRatio: false,
                    plugins: {
                        legend: {
                            labels: {
                                color: '#e2e8f0'
                            }
                        }
                    },
                    scales: {
                        y: {
                            beginAtZero: true,
                            grid: {
                                color: 'rgba(255, 255, 255, 0.1)'
                            },
                            ticks: {
                                color: '#94a3b8'
                            }
                        },
                        x: {
                            grid: {
                                color: 'rgba(255, 255, 255, 0.1)'
                            },
                            ticks: {
                                color: '#94a3b8'
                            }
                        }
                    }
                }
            }
        );
    }

    updateCharts() {
        if (!this.data) return;

        const topPlayers = Array.isArray(this.data.topPlayers) ? this.data.topPlayers : [];

        // Update Top 10 Players chart
        if (topPlayers.length) {
            this.charts.scoreDistribution.data.labels = topPlayers.map(player => player.username || 'Player');
            this.charts.scoreDistribution.data.datasets[0].data = topPlayers.map(player => player.score || 0);
            this.charts.scoreDistribution.update();
        }

        // Update Performance Trends
        if (this.data.performanceTrends) {
            this.charts.performanceTrends.data.labels = this.data.performanceTrends.labels || [];
            this.charts.performanceTrends.data.datasets[0].data = this.data.performanceTrends.data || [];
            this.charts.performanceTrends.update();
        }

        // Update Rank Distribution
        if (this.data.rankDistribution) {
            this.charts.rankDistribution.data.datasets[0].data = this.data.rankDistribution || [];
            this.charts.rankDistribution.update();
        }

        // Update Activity Timeline
        if (this.data.activityTimeline) {
            this.charts.activityTimeline.data.labels = this.data.activityTimeline.labels || [];
            this.charts.activityTimeline.data.datasets[0].data = this.data.activityTimeline.data || [];
            this.charts.activityTimeline.update();
        }
    }

    updateStatsCards() {
        if (!this.data.stats) return;

        const stats = this.data.stats;

        this.updateStatValue('total-players', stats.totalPlayers || 0);
        this.updateStatValue('total-matches', stats.totalGames || 0);
        this.updateStatValue('avg-session', `${Math.max(1, Math.round((stats.averageScore || 0) / 100))}m`);
        this.updateStatValue('records-broken', this.data.recordsBroken || 0);

        this.updateStatValue('db-players', `${this.data.dbPlayers || stats.totalPlayers || 0} rows`);
        this.updateStatValue('db-matches', `${this.data.dbMatches || stats.totalGames || 0} rows`);
        this.updateStatValue('db-activity', `${this.data.dbActivity || 0} entries`);
        this.updateStatValue('db-size', `${(this.data.dbSize || 0).toFixed(2)} MB`);
    }

    updateStatValue(id, value) {
        const element = document.getElementById(id);
        if (element) {
            element.textContent = value;
        }
    }

    showError(message) {
        // Show error in a chart container
        const chartCanvas = document.getElementById('score-distribution-chart');
        if (!chartCanvas || !chartCanvas.parentElement) return;
        const container = chartCanvas.parentElement;
        container.innerHTML = `
            <div class="chart-error">
                <h4>⚠️ Error</h4>
                <p>${message}</p>
                <button class="btn btn-primary" onclick="statsDashboard.loadData()">Retry</button>
            </div>
        `;
    }

    destroy() {
        Object.values(this.charts).forEach(chart => {
            if (chart) {
                chart.destroy();
            }
        });
        this.charts = {};
        this.isInitialized = false;
    }
}

// Initialize stats dashboard when page loads
let statsDashboard;

document.addEventListener('DOMContentLoaded', function() {
    statsDashboard = new StatsDashboard();
    statsDashboard.init();
});

// Cleanup on page unload
window.addEventListener('beforeunload', function() {
    if (statsDashboard) {
        statsDashboard.destroy();
    }
});