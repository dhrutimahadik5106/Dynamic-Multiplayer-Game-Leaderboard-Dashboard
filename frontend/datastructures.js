// datastructures.js - Data Structures Visualization

class DataStructuresDashboard {
    constructor() {
        this.data = {};
        this.chart = null;
        this.isInitialized = false;
    }

    init() {
        if (this.isInitialized) return;

        this.loadData();
        this.isInitialized = true;

        // Auto-refresh every 2 seconds
        setInterval(() => {
            this.loadData();
        }, 2000);
    }

    async loadData() {
        try {
            const response = await fetch('/datastructures');
            if (!response.ok) {
                throw new Error('Failed to load data structures data');
            }
            const data = await response.json();
            this.data = data.datastructures;
            this.updateDisplay();
            this.updateChart();
        } catch (error) {
            console.error('Error loading data structures data:', error);
            this.showError('Failed to load data structures data');
        }
    }

    updateDisplay() {
        if (!this.data) return;

        // Update stat cards
        this.updateStatValue('avl-height', this.data.avlTreeHeight || 0);
        this.updateStatValue('heap-size', this.data.maxHeapSize || 0);
        this.updateStatValue('hash-size', this.data.hashTableSize || 0);
        this.updateStatValue('activity-size', this.data.activityLogSize || 0);
    }

    updateStatValue(elementId, value) {
        const element = document.getElementById(elementId);
        if (element) {
            element.textContent = value;
            element.classList.add('updating');
            setTimeout(() => element.classList.remove('updating'), 300);
        }
    }

    updateChart() {
        const container = d3.select('#ds-chart');
        container.selectAll('*').remove();

        if (!this.data) return;

        const data = [
            { name: 'AVL Tree Height', value: this.data.avlTreeHeight || 0, color: '#00d4ff' },
            { name: 'Max Heap Size', value: this.data.maxHeapSize || 0, color: '#ff6b6b' },
            { name: 'Hash Table Size', value: this.data.hashTableSize || 0, color: '#4ecdc4' },
            { name: 'Activity Log Size', value: this.data.activityLogSize || 0, color: '#ffe66d' }
        ];

        const margin = { top: 20, right: 30, bottom: 40, left: 60 };
        const width = 600 - margin.left - margin.right;
        const height = 300 - margin.top - margin.bottom;

        const svg = container
            .append('svg')
            .attr('width', width + margin.left + margin.right)
            .attr('height', height + margin.top + margin.bottom)
            .append('g')
            .attr('transform', `translate(${margin.left},${margin.top})`);

        // Scales
        const x = d3.scaleBand()
            .domain(data.map(d => d.name))
            .range([0, width])
            .padding(0.1);

        const y = d3.scaleLinear()
            .domain([0, d3.max(data, d => d.value)])
            .nice()
            .range([height, 0]);

        // Bars
        svg.selectAll('.bar')
            .data(data)
            .enter().append('rect')
            .attr('class', 'bar')
            .attr('x', d => x(d.name))
            .attr('y', d => y(d.value))
            .attr('width', x.bandwidth())
            .attr('height', d => height - y(d.value))
            .attr('fill', d => d.color)
            .attr('rx', 4);

        // Axes
        svg.append('g')
            .attr('transform', `translate(0,${height})`)
            .call(d3.axisBottom(x))
            .selectAll('text')
            .style('text-anchor', 'middle')
            .style('fill', '#e2e8f0')
            .style('font-size', '12px');

        svg.append('g')
            .call(d3.axisLeft(y))
            .selectAll('text')
            .style('fill', '#e2e8f0')
            .style('font-size', '12px');

        // Grid lines
        svg.append('g')
            .attr('class', 'grid')
            .call(d3.axisLeft(y)
                .tickSize(-width)
                .tickFormat('')
            )
            .style('stroke', '#374151')
            .style('stroke-opacity', 0.3);

        // Value labels on bars
        svg.selectAll('.value-label')
            .data(data)
            .enter().append('text')
            .attr('class', 'value-label')
            .attr('x', d => x(d.name) + x.bandwidth() / 2)
            .attr('y', d => y(d.value) - 5)
            .attr('text-anchor', 'middle')
            .style('fill', '#e2e8f0')
            .style('font-size', '14px')
            .style('font-weight', 'bold')
            .text(d => d.value);
    }

    showError(message) {
        const container = document.querySelector('.main-content');
        if (!container) return;

        // Remove existing error
        const existingError = container.querySelector('.error-message');
        if (existingError) existingError.remove();

        const errorDiv = document.createElement('div');
        errorDiv.className = 'error-message card';
        errorDiv.innerHTML = `
            <div class="card-content">
                <h4>⚠️ Error</h4>
                <p>${message}</p>
                <button class="btn btn-primary" onclick="dsDashboard.loadData()">Retry</button>
            </div>
        `;

        container.insertBefore(errorDiv, container.firstChild.nextSibling);
    }
}

// Initialize data structures dashboard when page loads
let dsDashboard;

document.addEventListener('DOMContentLoaded', function() {
    dsDashboard = new DataStructuresDashboard();
    dsDashboard.init();
});