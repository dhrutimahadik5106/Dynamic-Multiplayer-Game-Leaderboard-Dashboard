class StatsDashboard {
    constructor() {
        this.charts = {};
        this.state = null;
        this.refreshInterval = null;
        this.backendAvailable = true;
    }

    async init() {
        console.log('[Stats] Initializing dashboard...');
        this.createCharts();
        
        // Try to load from backend first
        const loaded = await this.loadFromBackend();
        if (!loaded) {
            console.log('[Stats] Backend unavailable, using frontend simulation');
            this.initializeSimulation();
        }
        
        this.renderAll();
        
        // Auto-refresh every 5 seconds
        this.refreshInterval = setInterval(async () => {
            if (this.backendAvailable) {
                await this.loadFromBackend();
            } else {
                this.simulateUpdate();
            }
            this.renderAll();
        }, 5000);
        
        console.log('[Stats] Dashboard ready');
    }

    async loadFromBackend() {
        try {
            console.log('[Stats] Fetching from /stats endpoint...');
            const API_BASE = (window.location.origin && window.location.origin !== 'null') 
                ? window.location.origin 
                : 'http://localhost:8080';
            
            const response = await fetch(`${API_BASE}/stats`);
            if (!response.ok) {
                throw new Error(`HTTP ${response.status}`);
            }
            
            const data = await response.json();
            console.log('[Stats] Backend response:', data);
            console.log('STATS DATA:', data);
            if (!data || !data.trends || !Array.isArray(data.timeLabels)) {
                console.error('[Stats] Invalid backend payload', data);
                throw new Error('Invalid stats payload');
            }
            
            // Validate and transform backend response
            this.state = this.normalizeBackendData(data);
            console.log('[Stats] Normalized state:', this.state);
            
            this.backendAvailable = true;
            return true;
            
        } catch (error) {
            console.error('[Stats] Backend fetch error:', error);
            this.backendAvailable = false;
            return false;
        }
    }

    normalizeBackendData(data) {
        // Ensure all required fields exist with proper structure
        const normalized = {
            totalPlayers: data.totalPlayers ?? 0,
            totalMatches: data.totalMatches ?? 0,
            avgSession: Math.round(data.avgSessionTime ?? 0),
            recordsBroken: data.recordsBroken ?? 0,
            
            topPlayers: Array.isArray(data.topPlayers) 
                ? data.topPlayers.map((p, i) => ({
                    username: p.username ?? `Player ${i+1}`,
                    score: p.score ?? 0,
                    change: 0,
                    rank: i + 1
                }))
                : [],
            
            rankDistribution: {
                top10: data.rankDistribution?.top10 ?? 0,
                top25: data.rankDistribution?.top25 ?? 0,
                top50: data.rankDistribution?.top50 ?? 0,
                bottom50: data.rankDistribution?.bottom50 ?? 0
            },
            
            timeLabels: Array.isArray(data.timeLabels) ? data.timeLabels : [],
            
            trends: {
                avgScore: Array.isArray(data.trends?.avgScore) ? data.trends.avgScore : [],
                topScore: Array.isArray(data.trends?.topScore) ? data.trends.topScore : [],
                activePlayers: Array.isArray(data.trends?.activePlayers) ? data.trends.activePlayers : []
            },
            
            activityTimeline: Array.isArray(data.activityTimeline) ? data.activityTimeline : []
        };
        
        // Validate array lengths match
        const expectedLen = normalized.timeLabels.length;
        if (expectedLen > 0) {
            ['avgScore', 'topScore', 'activePlayers'].forEach(key => {
                while (normalized.trends[key].length < expectedLen) {
                    normalized.trends[key].push(0);
                }
                normalized.trends[key] = normalized.trends[key].slice(-expectedLen);
            });

            while (normalized.activityTimeline.length < expectedLen) {
                normalized.activityTimeline.unshift(0);
            }
            normalized.activityTimeline = normalized.activityTimeline.slice(-expectedLen);
        }
        
        return normalized;
    }

    initializeSimulation() {
        const names = [
            'ShadowFox', 'NovaRift', 'PixelFury', 'BladeDrift', 'CyberPulse',
            'VortexAce', 'EchoStrike', 'NeonViper', 'QuantumJet', 'TitanFlux'
        ];

        const topPlayers = names.map((name, index) => ({
            username: name,
            score: 840 + (10 - index) * 45 + Math.floor(Math.random() * 80),
            change: 0,
            rank: index + 1
        }));

        this.state = {
            totalPlayers: 12840,
            totalMatches: 5432,
            avgSession: 28,
            recordsBroken: 37,
            rankDistribution: { top10: 12, top25: 29, top50: 53, bottom50: 47 },
            topPlayers,
            timeLabels: [],
            trends: { avgScore: [], topScore: [], activePlayers: [] },
            activityTimeline: []
        };

        for (let i = 0; i < 10; i += 1) {
            this.pushTrendData();
            this.pushActivityData();
        }
    }

    createCharts() {
        this.destroyCharts();

        this.charts.rankDistribution = new Chart(
            document.getElementById('rank-distribution-chart'), {
                type: 'bar',
                data: {
                    labels: ['Top 10%', 'Top 25%', 'Top 50%', 'Bottom 50%'],
                    datasets: [{
                        label: 'Players',
                        data: [0, 0, 0, 0],
                        backgroundColor: ['#22d3ee', '#8b5cf6', '#34d399', '#f97316'],
                        borderColor: ['#0ea5e9', '#7c3aed', '#10b981', '#f59e0b'],
                        borderWidth: 2,
                        borderRadius: 12,
                        borderSkipped: false
                    }]
                },
                options: {
                    responsive: true,
                    maintainAspectRatio: false,
                    plugins: {
                        legend: { display: false },
                        tooltip: { enabled: true }
                    },
                    scales: {
                        x: {
                            ticks: { color: '#cbd5e1', font: { weight: '600' } },
                            grid: { display: false }
                        },
                        y: {
                            beginAtZero: true,
                            ticks: { color: '#cbd5e1' },
                            grid: { color: 'rgba(148, 163, 184, 0.15)' }
                        }
                    }
                }
            }
        );

        this.charts.performanceTrends = new Chart(
            document.getElementById('performance-trends-chart'), {
                type: 'line',
                data: {
                    labels: [],
                    datasets: [
                        {
                            label: 'Average Score',
                            data: [],
                            borderColor: '#38bdf8',
                            backgroundColor: 'rgba(56, 189, 248, 0.18)',
                            tension: 0.35,
                            fill: true,
                            pointRadius: 3,
                            pointBackgroundColor: '#38bdf8'
                        },
                        {
                            label: 'Top Score',
                            data: [],
                            borderColor: '#22c55e',
                            backgroundColor: 'rgba(34, 197, 94, 0.18)',
                            tension: 0.35,
                            fill: true,
                            pointRadius: 3,
                            pointBackgroundColor: '#22c55e'
                        },
                        {
                            label: 'Players',
                            data: [],
                            borderColor: '#f59e0b',
                            backgroundColor: 'rgba(245, 158, 11, 0.18)',
                            tension: 0.35,
                            fill: true,
                            pointRadius: 3,
                            pointBackgroundColor: '#f59e0b',
                            yAxisID: 'y1'
                        }
                    ]
                },
                options: {
                    responsive: true,
                    maintainAspectRatio: false,
                    plugins: {
                        legend: { labels: { color: '#e2e8f0' } },
                        tooltip: { mode: 'index', intersect: false }
                    },
                    interaction: { mode: 'nearest', intersect: false },
                    scales: {
                        x: {
                            ticks: { color: '#cbd5e1' },
                            grid: { color: 'rgba(148, 163, 184, 0.12)' }
                        },
                        y: {
                            beginAtZero: true,
                            ticks: { color: '#cbd5e1' },
                            grid: { color: 'rgba(148, 163, 184, 0.12)' }
                        },
                        y1: {
                            position: 'right',
                            beginAtZero: true,
                            ticks: { color: '#f59e0b' },
                            grid: { display: false }
                        }
                    }
                }
            }
        );

        this.charts.activityTimeline = new Chart(
            document.getElementById('activity-timeline-chart'), {
                type: 'line',
                data: {
                    labels: [],
                    datasets: [
                        {
                            label: 'Activity Events',
                            data: [],
                            borderColor: '#38bdf8',
                            backgroundColor: 'rgba(56, 189, 248, 0.18)',
                            tension: 0.35,
                            fill: true,
                            pointRadius: 3,
                            pointBackgroundColor: '#38bdf8'
                        }
                    ]
                },
                options: {
                    responsive: true,
                    maintainAspectRatio: false,
                    plugins: {
                        legend: { labels: { color: '#e2e8f0' } },
                        tooltip: { mode: 'index', intersect: false }
                    },
                    interaction: { mode: 'nearest', intersect: false },
                    scales: {
                        x: {
                            ticks: { color: '#cbd5e1' },
                            grid: { color: 'rgba(148, 163, 184, 0.12)' }
                        },
                        y: {
                            beginAtZero: true,
                            ticks: { color: '#cbd5e1' },
                            grid: { color: 'rgba(148, 163, 184, 0.12)' }
                        }
                    }
                }
            }
        );
    }

    pushTrendData() {
        const now = new Date();
        this.state.timeLabels.push(now.toLocaleTimeString('en-US', { hour12: false, minute: '2-digit', second: '2-digit' }));
        if (this.state.timeLabels.length > 10) this.state.timeLabels.shift();

<<<<<<< HEAD
        const prevAvg = this.state.trends.avgScore.slice(-1)[0] ?? 420;
        const prevTop = this.state.trends.topScore.slice(-1)[0] ?? 920;
        const prevPlayers = this.state.trends.activePlayers.slice(-1)[0] ?? this.state.totalPlayers;
=======
        const topPlayers = Array.isArray(this.data.topPlayers) ? this.data.topPlayers : [];

        // Update Top 10 Players chart
        if (topPlayers.length) {
            this.charts.scoreDistribution.data.labels = topPlayers.map(player => player.username || 'Player');
            this.charts.scoreDistribution.data.datasets[0].data = topPlayers.map(player => player.score || 0);
            this.charts.scoreDistribution.update();
        }
>>>>>>> 32006fd7b88c7d28eff3f2ec53b06997290b94c7

        const nextAvg = Math.max(320, Math.min(480, prevAvg + this.randomInt(-8, 12)));
        const nextTop = Math.max(860, Math.min(990, prevTop + this.randomInt(-12, 24)));
        const nextPlayers = Math.max(12200, Math.min(13700, prevPlayers + this.randomInt(-18, 28)));

        this.state.trends.avgScore.push(nextAvg);
        this.state.trends.topScore.push(nextTop);
        this.state.trends.activePlayers.push(nextPlayers);

        if (this.state.trends.avgScore.length > 10) this.state.trends.avgScore.shift();
        if (this.state.trends.topScore.length > 10) this.state.trends.topScore.shift();
        if (this.state.trends.activePlayers.length > 10) this.state.trends.activePlayers.shift();
    }

    pushActivityData() {
        const joins = this.randomInt(12, 27);
        const matches = this.randomInt(8, 22);
        const scoreUpdates = this.randomInt(24, 52);
        const totalEvents = joins + matches + scoreUpdates;

        this.state.activityTimeline.joins.push(joins);
        this.state.activityTimeline.matches.push(matches);
        this.state.activityTimeline.scoreUpdates.push(scoreUpdates);
        this.state.activityTimeline.totalEvents.push(totalEvents);

        if (this.state.activityTimeline.joins.length > 10) this.state.activityTimeline.joins.shift();
        if (this.state.activityTimeline.matches.length > 10) this.state.activityTimeline.matches.shift();
        if (this.state.activityTimeline.scoreUpdates.length > 10) this.state.activityTimeline.scoreUpdates.shift();
        if (this.state.activityTimeline.totalEvents.length > 10) this.state.activityTimeline.totalEvents.shift();
    }

    randomInt(min, max) {
        return Math.floor(Math.random() * (max - min + 1)) + min;
    }

    simulateUpdate() {
        if (!this.state) return;
        
        this.state.totalPlayers += this.randomInt(6, 16);
        this.state.totalMatches += this.randomInt(2, 8);
        const sessionDelta = this.randomInt(-1, 2);
        this.state.avgSession = Math.max(10, Math.min(40, this.state.avgSession + sessionDelta));
        if (Math.random() < 0.35) this.state.recordsBroken += 1;

        this.state.rankDistribution.top10 = Math.max(8, Math.min(18, this.state.rankDistribution.top10 + this.randomInt(-1, 1)));
        this.state.rankDistribution.top25 = Math.max(23, Math.min(33, this.state.rankDistribution.top25 + this.randomInt(-1, 2)));
        this.state.rankDistribution.top50 = Math.max(45, Math.min(58, this.state.rankDistribution.top50 + this.randomInt(-1, 1)));
        this.state.rankDistribution.bottom50 = 100 - this.state.rankDistribution.top50;

        this.state.topPlayers = this.state.topPlayers.map((player) => {
            const delta = this.randomInt(-28, 52);
            return {
                ...player,
                score: Math.max(780, player.score + delta),
                change: delta
            };
        }).sort((a, b) => b.score - a.score).map((player, index) => ({ ...player, rank: index + 1 }));

        this.pushTrendData();
        this.pushActivityData();
    }

    renderAll() {
        if (!this.state) return;
        this.updateStatsCards();
        this.updateTopPlayers();
        this.updateCharts();
    }

    updateStatsCards() {
<<<<<<< HEAD
        if (!this.state) return;
        this.animateNumber('total-players', this.state.totalPlayers);
        this.animateNumber('total-matches', this.state.totalMatches);
        this.animateNumber('avg-session', this.state.avgSession, 'm');
        this.animateNumber('records-broken', this.state.recordsBroken);
=======
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
>>>>>>> 32006fd7b88c7d28eff3f2ec53b06997290b94c7
    }

    animateNumber(id, target, suffix = '') {
        const el = document.getElementById(id);
        if (!el) return;
        const current = Number(el.dataset.value ?? el.textContent.replace(/\D/g, '')) || 0;
        const end = Number(target);
        el.dataset.value = end;
        el.classList.add('stat-updating');

        const startTime = performance.now();
        const duration = 700;
        const step = (time) => {
            const progress = Math.min((time - startTime) / duration, 1);
            const value = Math.round(current + (end - current) * this.easeOutQuad(progress));
            el.textContent = suffix ? `${value}${suffix}` : value.toLocaleString();
            if (progress < 1) requestAnimationFrame(step);
            else setTimeout(() => el.classList.remove('stat-updating'), 300);
        };
        requestAnimationFrame(step);
    }

<<<<<<< HEAD
    easeOutQuad(t) {
        return t * (2 - t);
=======
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
>>>>>>> 32006fd7b88c7d28eff3f2ec53b06997290b94c7
    }

    updateTopPlayers() {
        const list = document.getElementById('top-players-list');
        if (!list || !this.state || !Array.isArray(this.state.topPlayers)) return;

        list.innerHTML = this.state.topPlayers.map((player) => {
            const rankLabel = player.rank === 1 ? '🥇' : player.rank === 2 ? '🥈' : player.rank === 3 ? '🥉' : `#${player.rank}`;
            const changeClass = player.change >= 0 ? 'trend-up' : 'trend-down';
            const sign = player.change >= 0 ? '▲' : '▼';
            return `
                <div class="player-entry">
                    <span class="player-rank">${rankLabel}</span>
                    <span class="player-name">${player.username}</span>
                    <span class="player-meta">
                        <span class="player-score">${player.score.toLocaleString()}</span>
                        <span class="player-trend ${changeClass}">${sign} ${Math.abs(player.change)}</span>
                    </span>
                </div>`;
        }).join('');
    }

    updateCharts() {
        if (!this.state) return;

        // Update rank distribution
        const dist = this.state.rankDistribution;
        this.charts.rankDistribution.data.datasets[0].data = [
            dist.top10,
            dist.top25,
            dist.top50,
            dist.bottom50
        ];
        this.charts.rankDistribution.update('none');

        // Update performance trends
        this.charts.performanceTrends.data.labels = this.state.timeLabels;
        this.charts.performanceTrends.data.datasets[0].data = this.state.trends.avgScore || [];
        this.charts.performanceTrends.data.datasets[1].data = this.state.trends.topScore || [];
        this.charts.performanceTrends.data.datasets[2].data = this.state.trends.activePlayers || [];
        this.charts.performanceTrends.update('none');

        // Update activity timeline
        this.charts.activityTimeline.data.labels = this.state.timeLabels;
        this.charts.activityTimeline.data.datasets[0].data = Array.isArray(this.state.activityTimeline) ? this.state.activityTimeline : [];
        this.charts.activityTimeline.update('none');
    }

    destroyCharts() {
        Object.values(this.charts).forEach((chart) => chart?.destroy?.());
        this.charts = {};
    }

    stop() {
        clearInterval(this.refreshInterval);
        this.destroyCharts();
    }
}

let statsDashboard;
window.addEventListener('DOMContentLoaded', () => {
    statsDashboard = new StatsDashboard();
    statsDashboard.init();
});

window.addEventListener('beforeunload', () => {
    statsDashboard?.stop();
});
