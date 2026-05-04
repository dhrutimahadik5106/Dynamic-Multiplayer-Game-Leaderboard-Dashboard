// activity.js - Activity Feed Page

const API_BASE = 'http://localhost:8080';

let allActivities = [];
let currentFilter = 'all';

async function loadActivityData() {
    try {
        // Fetch activity from backend
        const response = await fetch(`${API_BASE}/activity`);
        if (!response.ok) throw new Error(`HTTP ${response.status}`);
        
        const data = await response.json();
        
        // Extract text from each activity object
        allActivities = (data.events || []).map(event => event.text || event);
        
        console.log("Activities loaded:", allActivities.length, allActivities);
        
        // Initial render
        renderActivityFeed();
        updateActivityStats();
        
    } catch (error) {
        console.error("Error loading activity data:", error);
        allActivities = [];
        updateActivityStats();
    }
}

function renderActivityFeed() {
    const feed = document.getElementById("activity-feed");
    if (!feed) return;
    
    feed.innerHTML = "";
    
    // Filter activities based on current filter
    let filtered = allActivities;
    
    if (currentFilter === 'score') {
        filtered = allActivities.filter(a => /score|gained|won|points|updated/i.test(a));
    } else if (currentFilter === 'player') {
        filtered = allActivities.filter(a => /joined|new player/i.test(a));
    } else if (currentFilter === 'match') {
        filtered = allActivities.filter(a => /match|game|played|victory/i.test(a));
    }
    
    if (filtered.length === 0) {
        feed.innerHTML = '<div class="activity-item">No activities for this filter</div>';
        return;
    }
    
    filtered.slice(0, 20).forEach(activity => {
        const div = document.createElement("div");
        div.className = "activity-item";
        div.textContent = activity;
        
        // Add color coding based on activity type
        if (/score|gained|won|points|updated/i.test(activity)) {
            div.classList.add('activity-gain');
        } else if (/lost|defeated/i.test(activity)) {
            div.classList.add('activity-loss');
        } else if (/joined|new|started/i.test(activity)) {
            div.classList.add('activity-player');
        }
        
        feed.appendChild(div);
    });
}

function updateActivityStats() {
    const eventsEl = document.getElementById("events-today");
    const scoreEl = document.getElementById("score-changes");
    const playerEl = document.getElementById("new-players");
    const matchEl = document.getElementById("matches-played");
    
    const scoreCount = allActivities.filter(a => /score|gained|won|points|updated/i.test(a)).length;
    const playerCount = allActivities.filter(a => /joined|new player|started/i.test(a)).length;
    const matchCount = allActivities.filter(a => /match|game|played|victory/i.test(a)).length;
    
    if (eventsEl) eventsEl.textContent = allActivities.length;
    if (scoreEl) scoreEl.textContent = scoreCount;
    if (playerEl) playerEl.textContent = playerCount;
    if (matchEl) matchEl.textContent = matchCount;
    
    console.log("Stats updated - Total:", allActivities.length, "Score:", scoreCount, "Players:", playerCount, "Matches:", matchCount);
}

function setupFilterButtons() {
    const filterButtons = document.querySelectorAll('.filter-btn');
    
    filterButtons.forEach(btn => {
        btn.addEventListener('click', (e) => {
            // Remove active class from all buttons
            filterButtons.forEach(b => b.classList.remove('active'));
            
            // Add active class to clicked button
            e.target.classList.add('active');
            
            // Update filter and re-render
            currentFilter = e.target.dataset.filter || 'all';
            renderActivityFeed();
            
            console.log("Filter changed to:", currentFilter);
        });
    });
}

function setupControlButtons() {
    const clearBtn = document.getElementById("clear-activity-btn");
    const refreshBtn = document.getElementById("refresh-activity-btn");
    
    if (clearBtn) {
        clearBtn.addEventListener('click', () => {
            allActivities = [];
            renderActivityFeed();
            updateActivityStats();
            console.log("Activity cleared");
        });
    }
    
    if (refreshBtn) {
        refreshBtn.addEventListener('click', () => {
            loadActivityData();
            console.log("Activity refreshed");
        });
    }
}

// Initialize on page load
document.addEventListener('DOMContentLoaded', () => {
    console.log("Activity page initialized");
    setupFilterButtons();
    setupControlButtons();
    loadActivityData();
    
    // Auto-refresh every 10 seconds
    setInterval(() => {
        loadActivityData();
    }, 10000);
});
