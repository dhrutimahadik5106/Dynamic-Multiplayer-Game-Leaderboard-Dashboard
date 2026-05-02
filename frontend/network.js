// network.js - Player Network Visualization

class NetworkGraph {
    constructor(svgId) {
        this.svgId = svgId;
        this.svg = null;
        this.simulation = null;
        this.nodes = [];
        this.links = [];
        this.width = 800;
        this.height = 600;
        this.colorScale = d3.scaleOrdinal(d3.schemeCategory10);
        this.sizeScale = d3.scaleLinear().range([10, 30]);
        this.isInitialized = false;
        this.tooltip = null;
    }

    init() {
        if (this.isInitialized) return;

        this.svg = d3.select(`#${this.svgId}`)
            .attr('viewBox', [0, 0, this.width, this.height]);

        // Add zoom behavior
        const zoom = d3.zoom()
            .scaleExtent([0.1, 4])
            .on('zoom', (event) => {
                this.svg.select('.network-group').attr('transform', event.transform);
            });

        this.svg.call(zoom);

        // Create main group
        this.svg.append('g').attr('class', 'network-group');

        // Add arrow marker for directed edges
        this.svg.append('defs').append('marker')
            .attr('id', 'arrowhead')
            .attr('viewBox', '-0 -5 10 10')
            .attr('refX', 20)
            .attr('refY', 0)
            .attr('orient', 'auto')
            .attr('markerWidth', 6)
            .attr('markerHeight', 6)
            .attr('xoverflow', 'visible')
            .append('svg:path')
            .attr('d', 'M 0,-5 L 10 ,0 L 0,5')
            .attr('fill', '#00d4ff')
            .style('opacity', 0.6);

        // Create tooltip
        this.tooltip = d3.select('body').append('div')
            .attr('class', 'network-tooltip')
            .style('position', 'absolute')
            .style('visibility', 'hidden')
            .style('background', 'rgba(0, 0, 0, 0.8)')
            .style('color', 'white')
            .style('padding', '8px')
            .style('border-radius', '4px')
            .style('font-size', '12px')
            .style('pointer-events', 'none')
            .style('z-index', '1000');

        this.isInitialized = true;
    }

    async loadData() {
        try {
            const response = await fetch('http://localhost:8080/api/network');
            if (!response.ok) {
                throw new Error('Failed to load network data');
            }
            const data = await response.json();

            // Limit to top 50 players by score
            this.nodes = data.nodes
                .sort((a, b) => b.score - a.score)
                .slice(0, 50);

            // Filter links to only include nodes that are in the top 50
            const nodeIds = new Set(this.nodes.map(n => n.id));
            this.links = data.links.filter(link =>
                nodeIds.has(link.source) && nodeIds.has(link.target)
            );

            // Update scales
            this.sizeScale.domain(d3.extent(this.nodes, d => d.score || 0));

            this.render();
        } catch (error) {
            console.error('Error loading network data:', error);
            this.showError('Failed to load network data');
        }
    }

    render() {
        if (!this.svg) return;

        const group = this.svg.select('.network-group');

        // Update scales
        this.sizeScale.domain(d3.extent(this.nodes, d => d.score || 0));

        // Create force simulation
        this.simulation = d3.forceSimulation(this.nodes)
            .force('link', d3.forceLink(this.links).id(d => d.id).distance(100))
            .force('charge', d3.forceManyBody().strength(-300))
            .force('center', d3.forceCenter(this.width / 2, this.height / 2))
            .force('collision', d3.forceCollide().radius(d => this.sizeScale(d.score || 0) + 5));

        // Create links
        const link = group.selectAll('.link')
            .data(this.links, d => `${d.source}-${d.target}`)
            .join(
                enter => enter.append('line')
                    .attr('class', 'link')
                    .attr('stroke', '#00d4ff')
                    .attr('stroke-opacity', 0.6)
                    .attr('stroke-width', d => Math.sqrt(d.value || 1))
                    .attr('marker-end', 'url(#arrowhead)'),
                update => update,
                exit => exit.remove()
            );

        // Create nodes
        const node = group.selectAll('.node')
            .data(this.nodes, d => d.id)
            .join(
                enter => enter.append('circle')
                    .attr('class', 'node')
                    .attr('r', d => this.sizeScale(d.score || 0))
                    .attr('fill', d => d.rank <= 5 ? '#ffd700' : '#00d4ff') // Gold for top 5
                    .attr('stroke', '#00d4ff')
                    .attr('stroke-width', 2)
                    .call(d3.drag()
                        .on('start', (event, d) => {
                            if (!event.active) this.simulation.alphaTarget(0.3).restart();
                            d.fx = d.x;
                            d.fy = d.y;
                        })
                        .on('drag', (event, d) => {
                            d.fx = event.x;
                            d.fy = event.y;
                        })
                        .on('end', (event, d) => {
                            if (!event.active) this.simulation.alphaTarget(0);
                            d.fx = null;
                            d.fy = null;
                        })
                    )
                    .on('mouseover', (event, d) => {
                        this.tooltip
                            .style('visibility', 'visible')
                            .html(`<strong>${d.name}</strong><br>Rank: ${d.rank}<br>Score: ${d.score}`);
                    })
                    .on('mousemove', (event) => {
                        this.tooltip
                            .style('top', (event.pageY - 10) + 'px')
                            .style('left', (event.pageX + 10) + 'px');
                    })
                    .on('mouseout', () => {
                        this.tooltip.style('visibility', 'hidden');
                    }),
                update => update
                    .attr('r', d => this.sizeScale(d.score || 0))
                    .attr('fill', d => d.rank <= 5 ? '#ffd700' : '#00d4ff'),
                exit => exit.remove()
            );

        // Add labels
        const labels = group.selectAll('.label')
            .data(this.nodes, d => d.id)
            .join(
                enter => enter.append('text')
                    .attr('class', 'label')
                    .text(d => d.name)
                    .attr('font-family', 'Rajdhani, sans-serif')
                    .attr('font-size', '12px')
                    .attr('fill', '#e2e8f0')
                    .attr('text-anchor', 'middle')
                    .attr('dy', d => this.sizeScale(d.score || 0) + 15),
                update => update
                    .text(d => d.name)
                    .attr('dy', d => this.sizeScale(d.score || 0) + 15),
                exit => exit.remove()
            );

        // Update positions on simulation tick
        this.simulation.on('tick', () => {
            link
                .attr('x1', d => d.source.x)
                .attr('y1', d => d.source.y)
                .attr('x2', d => d.target.x)
                .attr('y2', d => d.target.y);

            node
                .attr('cx', d => d.x)
                .attr('cy', d => d.y);

            labels
                .attr('x', d => d.x)
                .attr('y', d => d.y);
        });

        // Hide placeholder
        d3.select('.network-placeholder').style('display', 'none');
    }

    showError(message) {
        const container = d3.select('#network-container');
        container.select('.network-placeholder').remove();

        container.append('div')
            .attr('class', 'network-placeholder')
            .html(`
                <div class="placeholder-content">
                    <h4>⚠️ Error</h4>
                    <p>${message}</p>
                    <button class="btn btn-primary" onclick="networkGraph.loadData()">Retry</button>
                </div>
            `);
    }

    updateData(newNodes, newLinks) {
        this.nodes = newNodes || [];
        this.links = newLinks || [];
        this.render();
    }

    destroy() {
        if (this.simulation) {
            this.simulation.stop();
        }
        if (this.tooltip) {
            this.tooltip.remove();
        }
        this.isInitialized = false;
    }
}

// Initialize network graph when page loads
let networkGraph;

document.addEventListener('DOMContentLoaded', function() {
    networkGraph = new NetworkGraph('network-svg');
    networkGraph.init();
    networkGraph.loadData();

    // Auto-refresh every 5 seconds
    setInterval(() => {
        networkGraph.loadData();
    }, 5000);

    // Handle refresh button
    d3.select('#refresh-network-btn').on('click', () => {
        networkGraph.loadData();
    });
});

// Cleanup on page unload
window.addEventListener('beforeunload', function() {
    if (networkGraph) {
        networkGraph.destroy();
    }
});