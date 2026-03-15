// network.js - Player Network Visualization

class NetworkGraph {
    constructor(containerId) {
        this.containerId = containerId;
        this.svg = null;
        this.simulation = null;
        this.nodes = [];
        this.links = [];
        this.width = 800;
        this.height = 600;
        this.colorScale = d3.scaleOrdinal(d3.schemeCategory10);
        this.sizeScale = d3.scaleLinear().range([5, 20]);
        this.isInitialized = false;
    }

    init() {
        if (this.isInitialized) return;

        this.svg = d3.select(`#${this.containerId}`)
            .append('svg')
            .attr('width', this.width)
            .attr('height', this.height)
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

        this.isInitialized = true;
    }

    async loadData() {
        try {
            const response = await fetch('/matches');
            if (!response.ok) {
                throw new Error('Failed to load network data');
            }
            const links = await response.json();

            // Create nodes from unique players in links
            const nodeMap = new Map();
            links.forEach(link => {
                if (!nodeMap.has(link.source)) {
                    nodeMap.set(link.source, { id: link.source, name: link.source });
                }
                if (!nodeMap.has(link.target)) {
                    nodeMap.set(link.target, { id: link.target, name: link.target });
                }
            });

            this.nodes = Array.from(nodeMap.values());
            this.links = links;

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

        // Clear previous content
        group.selectAll('*').remove();

        if (this.nodes.length === 0) {
            this.showNoData();
            return;
        }

        // Create force simulation
        this.simulation = d3.forceSimulation(this.nodes)
            .force('link', d3.forceLink(this.links).id(d => d.id).distance(100))
            .force('charge', d3.forceManyBody().strength(-300))
            .force('center', d3.forceCenter(this.width / 2, this.height / 2))
            .force('collision', d3.forceCollide().radius(d => this.sizeScale(d.score || 0) + 5));

        // Create links
        const link = group.append('g')
            .attr('class', 'links')
            .selectAll('line')
            .data(this.links)
            .enter().append('line')
            .attr('stroke', '#00d4ff')
            .attr('stroke-opacity', 0.6)
            .attr('stroke-width', d => Math.sqrt(d.value || 1))
            .attr('marker-end', 'url(#arrowhead)');

        // Create nodes
        const node = group.append('g')
            .attr('class', 'nodes')
            .selectAll('circle')
            .data(this.nodes)
            .enter().append('circle')
            .attr('r', d => this.sizeScale(d.score || 0))
            .attr('fill', d => this.colorScale(d.rank || 0))
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
                }));

        // Add labels
        const labels = group.append('g')
            .attr('class', 'labels')
            .selectAll('text')
            .data(this.nodes)
            .enter().append('text')
            .text(d => d.name || d.id)
            .attr('font-family', 'Rajdhani, sans-serif')
            .attr('font-size', '12px')
            .attr('fill', '#e2e8f0')
            .attr('text-anchor', 'middle')
            .attr('dy', d => this.sizeScale(d.score || 0) + 15);

        // Add tooltips
        node.append('title')
            .text(d => `${d.name || d.id}\nRank: ${d.rank || 'N/A'}\nScore: ${d.score || 0}`);

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

        // Add controls
        this.addControls();
    }

    addControls() {
        const controls = d3.select(`#${this.containerId}`)
            .append('div')
            .attr('class', 'network-controls')
            .style('position', 'absolute')
            .style('top', '10px')
            .style('right', '10px');

        controls.append('button')
            .attr('class', 'btn btn-secondary btn-small')
            .text('🔄 Reset')
            .on('click', () => {
                this.simulation.alpha(1).restart();
            });

        controls.append('button')
            .attr('class', 'btn btn-secondary btn-small')
            .text('🎯 Center')
            .on('click', () => {
                this.svg.transition().duration(750).call(
                    d3.zoom().transform,
                    d3.zoomIdentity
                );
            });
    }

    showNoData() {
        const group = this.svg.select('.network-group');

        group.append('text')
            .attr('x', this.width / 2)
            .attr('y', this.height / 2 - 20)
            .attr('text-anchor', 'middle')
            .attr('fill', '#94a3b8')
            .attr('font-size', '18px')
            .text('No Network Data Available');

        group.append('text')
            .attr('x', this.width / 2)
            .attr('y', this.height / 2 + 20)
            .attr('text-anchor', 'middle')
            .attr('fill', '#94a3b8')
            .attr('font-size', '14px')
            .text('Add players to see their connections');
    }

    showError(message) {
        const container = d3.select(`#${this.containerId}`);
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
        if (this.svg) {
            this.svg.remove();
        }
        this.isInitialized = false;
    }
}

// Initialize network graph when page loads
let networkGraph;

document.addEventListener('DOMContentLoaded', function() {
    networkGraph = new NetworkGraph('network-container');
    networkGraph.init();
    networkGraph.loadData();

    // Auto-refresh every 30 seconds
    setInterval(() => {
        networkGraph.loadData();
    }, 30000);
});

// Cleanup on page unload
window.addEventListener('beforeunload', function() {
    if (networkGraph) {
        networkGraph.destroy();
    }
});