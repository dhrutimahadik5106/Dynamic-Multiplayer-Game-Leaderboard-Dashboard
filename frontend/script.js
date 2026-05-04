// =============================
// NAVIGATION SYSTEM
// =============================
function initNavigation() {
    const currentPath = window.location.pathname;
    const currentPage = currentPath.split('/').pop().replace('.html', '') || 'index';

    const navLinks = document.querySelectorAll('.nav-link');
    navLinks.forEach(link => {
        const href = link.getAttribute('href').replace('.html', '');
        if (href === currentPage) {
            link.classList.add('active');
        }
    });
}

// =============================
// GLOBAL VARIABLES
// =============================
let leaderboardData = [];
let updateInterval;
let dashboardInterval;
let activityInterval;
let previousPlayerPositions = new Map(); // Track previous positions for animation
let useLocalMode = false;

const API_BASE = (window.location.origin && window.location.origin !== 'null') ? window.location.origin : 'http://localhost:8080';
const LOCAL_STATE_KEY = 'leaderboard_local_state_v1';

function readLocalState() {
    const raw = localStorage.getItem(LOCAL_STATE_KEY);
    if (raw) {
        try {
            return JSON.parse(raw);
        } catch (e) {
            console.warn('Local state parse failed, resetting.', e);
        }
    }

    const seed = {
        nextId: 1,
        players: [],
        activity: []
    };
    localStorage.setItem(LOCAL_STATE_KEY, JSON.stringify(seed));
    return seed;
}

function writeLocalState(state) {
    localStorage.setItem(LOCAL_STATE_KEY, JSON.stringify(state));
}

function toSortedPlayers(players) {
    const copy = [...players];
    copy.sort((a, b) => b.score - a.score);
    return copy.map((p, i) => ({ ...p, rank: i + 1 }));
}

function localStatsFromPlayers(players) {
    const totalPlayers = players.length;
    const topScore = totalPlayers ? players[0].score : 0;
    const avgScore = totalPlayers ? players.reduce((s, p) => s + p.score, 0) / totalPlayers : 0;
    return {
        totalPlayers,
        topScore,
        avgScore,
        activeMatches: 25
    };
}

function isDashboardPage() {
    return !!document.getElementById('top-players') && !!document.getElementById('activity-feed');
}

async function fetchLeaderboard() {
    try {
        const leaderboardCard = document.querySelector('.leaderboard-card');
        if (leaderboardCard) {
            leaderboardCard.classList.add('updating');
        }

        const response = await fetch(`${API_BASE}/leaderboard`);
        const data = await response.json();

        console.log(`🔄 Leaderboard fetch: ${data.length} players`);
        console.log(`📊 Top 3: ${data.slice(0,3).map(p => `${p.username}:${p.score}`).join(', ')}`);

        leaderboardData = data;

        updateLeaderboardDisplay(leaderboardData);
        updateQuickStats(leaderboardData);
        updateTopPlayers(leaderboardData.slice(0, 5));

        updateLastUpdated();

        setTimeout(() => {
            if (leaderboardCard) {
                leaderboardCard.classList.remove('updating');
            }
        }, 500);

    } catch (error) {
        console.error("❌ Leaderboard fetch error:", error);
        useLocalMode = true;

        const local = readLocalState();
        const sorted = toSortedPlayers(local.players);
        leaderboardData = sorted;
        updateLeaderboardDisplay(sorted);
        updateQuickStats(sorted);
        updateTopPlayers(sorted.slice(0, 5));

        const leaderboardCard = document.querySelector('.leaderboard-card');
        if (leaderboardCard) {
            leaderboardCard.classList.remove('updating');
        }
    }
}

async function fetchDashboardStats() {
    try {
        const response = await fetch(`${API_BASE}/stats`);
        if (!response.ok) throw new Error(`Stats HTTP ${response.status}`);
        const stats = await response.json();

        const totalEl = document.getElementById('total-players');
        const topEl = document.getElementById('top-score');
        const avgEl = document.getElementById('avg-score');
        const activeEl = document.getElementById('active-matches');

        if (totalEl) totalEl.innerText = stats.totalPlayers ?? 0;
        if (topEl) topEl.innerText = stats.topScore ?? 0;
        if (avgEl) avgEl.innerText = Math.round(stats.avgScore ?? 0);
        if (activeEl) activeEl.innerText = stats.activeMatches ?? 0;

        console.log('Stats refreshed', stats);
    } catch (error) {
        console.error('Stats fetch error:', error);
        useLocalMode = true;
        const local = readLocalState();
        const sorted = toSortedPlayers(local.players);
        const stats = localStatsFromPlayers(sorted);

        const totalEl = document.getElementById('total-players');
        const topEl = document.getElementById('top-score');
        const avgEl = document.getElementById('avg-score');
        const activeEl = document.getElementById('active-matches');

        if (totalEl) totalEl.innerText = stats.totalPlayers;
        if (topEl) topEl.innerText = stats.topScore;
        if (avgEl) avgEl.innerText = Math.round(stats.avgScore);
        if (activeEl) activeEl.innerText = stats.activeMatches;
    }
}

async function fetchTopPlayers() {
    try {
        const response = await fetch(`${API_BASE}/topPlayers`);
        if (!response.ok) throw new Error(`Top players HTTP ${response.status}`);
        const players = await response.json();
        updateTopPlayers(Array.isArray(players) ? players : []);
    } catch (error) {
        console.error('Top players fetch error:', error);
        useLocalMode = true;
        const local = readLocalState();
        const sorted = toSortedPlayers(local.players);
        updateTopPlayers(sorted.slice(0, 5));
    }
}

async function fetchDashboard() {
    await Promise.all([
        fetchDashboardStats(),
        fetchTopPlayers(),
        fetchRecentActivity()
    ]);
}

function openPlayerModal() {
    const modal = document.getElementById('player-modal');
    if (!modal) return;
    modal.classList.add('open');
    setTimeout(() => {
        document.getElementById('player-name')?.focus();
    }, 100);
}

function closePlayerModal() {
    const modal = document.getElementById('player-modal');
    if (!modal) return;
    modal.classList.remove('open');
}

// =============================
// UPDATE TIMESTAMP
// =============================
function updateLastUpdated() {
    const updateTimeEl = document.getElementById('update-time');
    if (updateTimeEl) {
        const now = new Date();
        const timeString = now.toLocaleTimeString();
        updateTimeEl.textContent = timeString;
    }
}

// =============================
// FETCH ACTIVITY
// =============================
async function fetchRecentActivity() {
    try {
        const response = await fetch(`${API_BASE}/activity`);
        if (!response.ok) throw new Error(`Activity HTTP ${response.status}`);
        const data = await response.json();
        const events = Array.isArray(data) ? data : (Array.isArray(data.events) ? data.events.map(e => e.text) : []);
        updateActivityFeed(events);
        console.log("Activity feed updated", events.length, "items");

    } catch (error) {
        console.error("Activity fetch error:", error);
        useLocalMode = true;
        const local = readLocalState();
        updateActivityFeed(local.activity.slice(0, 10));
    }
}

// =============================
// UPDATE TABLE
// =============================
function updateLeaderboardDisplay(players) {

    const tbody = document.getElementById("leaderboard-body");
    if (!tbody) {
        console.warn("⚠️ No leaderboard-body found");
        return;
    }

    console.log(`🎯 Updating leaderboard display: ${players.length} players`);

    const rowHeight = 50; // Approximate row height

    // Store current DOM positions BEFORE any changes
    const currentDomPositions = new Map();
    tbody.querySelectorAll('.player-row').forEach((row, index) => {
        const playerId = parseInt(row.dataset.playerId);
        currentDomPositions.set(playerId, index);
    });

    // Create a map of existing rows by player ID
    const existingRows = new Map();
    tbody.querySelectorAll('.player-row').forEach(row => {
        const playerId = parseInt(row.dataset.playerId);
        existingRows.set(playerId, row);
    });

    // Track score changes for highlighting
    const scoreChanges = new Map();

    // Update or create rows
    players.forEach((player, newIndex) => {
        const playerId = player.id;
        const oldDomIndex = currentDomPositions.get(playerId);
        let row = existingRows.get(playerId);

        if (!row) {
            // Create new row
            row = createPlayerRow(player, newIndex + 1);
            row.classList.add('new-row');
            tbody.appendChild(row);
            // Remove animation class after it completes
            setTimeout(() => row.classList.remove('new-row'), 600);
        } else {
            // Update existing row content
            const oldScore = parseInt(row.querySelector('.score-cell').textContent.replace(/[^\d-]/g, ''));
            updateRowContent(row, player, newIndex + 1);

            // Track score change for highlighting
            if (!isNaN(oldScore) && oldScore !== player.score) {
                scoreChanges.set(playerId, player.score - oldScore);
            }
        }

        // Set data attributes
        row.dataset.playerId = playerId;
        row.dataset.rank = newIndex + 1;

        // Calculate and apply movement animation
        if (oldDomIndex !== undefined && oldDomIndex !== newIndex) {
            const deltaY = (oldDomIndex - newIndex) * rowHeight;
            console.log(`🎬 Animating ${player.username}: position ${oldDomIndex} → ${newIndex} (delta: ${deltaY}px)`);
            row.style.transform = `translateY(${deltaY}px)`;
            row.style.transition = 'transform 0.6s ease';

            // Reset transform after animation
            setTimeout(() => {
                row.style.transform = '';
                row.style.transition = '';
            }, 600);
        }
    });

    // Reorder DOM after all animations are set up
    setTimeout(() => {
        const allRows = Array.from(tbody.children);
        const sortedRows = allRows.sort((a, b) => {
            const rankA = parseInt(a.dataset.rank) || 999;
            const rankB = parseInt(b.dataset.rank) || 999;
            return rankA - rankB;
        });

        // Rebuild tbody with sorted rows
        tbody.innerHTML = '';
        sortedRows.forEach(r => tbody.appendChild(r));
    }, 650);

    // Apply score change highlights
    scoreChanges.forEach((change, playerId) => {
        const row = existingRows.get(playerId);
        if (row) {
            const scoreCell = row.querySelector('.score-cell');
            if (scoreCell) {
                if (change > 0) {
                    scoreCell.classList.add('score-up');
                    setTimeout(() => scoreCell.classList.remove('score-up'), 1000);
                } else if (change < 0) {
                    scoreCell.classList.add('score-down');
                    setTimeout(() => scoreCell.classList.remove('score-down'), 1000);
                }
            }
        }
    });

    // Remove rows for players no longer in list
    existingRows.forEach((row, playerId) => {
        if (!players.some(p => p.id === playerId)) {
            row.style.opacity = '0';
            setTimeout(() => {
                if (row.parentNode) row.parentNode.removeChild(row);
            }, 300);
        }
    });

    // Update previous positions for next animation cycle
    previousPlayerPositions.clear();
    players.forEach((player, index) => {
        previousPlayerPositions.set(player.id, index);
    });
}

function createPlayerRow(player, rank) {
    const row = document.createElement("tr");
    row.className = "player-row";

    row.innerHTML = `
        <td class="rank-cell">#${rank}</td>
        <td class="player-name">${player.username}</td>
        <td class="score-cell">${player.score}</td>
        <td class="change-cell">—</td>
        <td>online</td>
        <td>
            <button onclick="updatePlayerScore(${player.id},10)">+10</button>
            <button onclick="updatePlayerScore(${player.id},-5)">-5</button>
        </td>
    `;

    return row;
}

function updateRowContent(row, player, rank) {
    const cells = row.querySelectorAll('td');
    if (cells.length >= 3) {
        cells[0].textContent = `#${rank}`;
        cells[1].textContent = player.username;
        cells[2].textContent = player.score;
    }
}

function createFloatingScoreChange(row, change, direction) {
    const scoreCell = row.querySelector('.score-cell');
    if (!scoreCell) return;

    const floatDiv = document.createElement('div');
    floatDiv.className = `score-change-float ${direction}`;
    floatDiv.textContent = change > 0 ? `+${change}` : change;
    floatDiv.style.left = `${scoreCell.offsetLeft + scoreCell.offsetWidth / 2}px`;
    floatDiv.style.top = `${scoreCell.offsetTop}px`;

    row.style.position = 'relative';
    row.appendChild(floatDiv);

    setTimeout(() => {
        if (floatDiv.parentNode) {
            floatDiv.parentNode.removeChild(floatDiv);
        }
    }, 1500);
}

// =============================
// TOP PLAYERS
// =============================
let previousTopPlayers = [];

function updateTopPlayers(players) {
    const container = document.getElementById("top-players");
    if (!container) return;

    const changes = players.map((p, index) => {
        const prev = previousTopPlayers[index];
        return {
            player: p,
            changed: !prev || prev.id !== p.id || prev.score !== p.score,
            scoreChange: prev ? p.score - prev.score : 0
        };
    });

    previousTopPlayers = [...players];
    container.innerHTML = "";

    players.forEach((p, index) => {
        const row = document.createElement("div");
        row.className = "top-player-item";

        const badge = document.createElement("span");
        badge.textContent = index < 3 ? ["🥇", "🥈", "🥉"][index] : `#${index+1}`;

        const name = document.createElement("span");
        name.textContent = p.username;

        const score = document.createElement("span");
        score.textContent = p.score;

        row.appendChild(badge);
        row.appendChild(name);
        row.appendChild(score);

        const change = changes[index];
        if (change.changed && change.scoreChange !== 0) {
            row.classList.add(change.scoreChange > 0 ? 'score-up' : 'score-down');
            setTimeout(() => row.classList.remove('score-up', 'score-down'), 1000);
        }

        container.appendChild(row);
    });

    if (!players.length) {
        container.innerHTML = '<div class="top-player-item">No players available yet</div>';
    }

    console.log("Top players updated");
}

// =============================
// ACTIVITY FEED
// =============================
function updateActivityFeed(data) {
    const feed = document.getElementById("activity-feed");
    if (!feed) return;

    feed.innerHTML = "";

    if (!data || !data.length) {
        feed.innerHTML = '<div class="activity-item">No recent activity yet</div>';
        return;
    }

    data.slice(0, 10).forEach(item => {
        const div = document.createElement("div");
        div.className = "activity-item";
        div.textContent = item;

        if (/\+\d+|gained|won/i.test(item)) {
            div.classList.add('activity-gain');
        } else if (/\-|lost|lost/i.test(item)) {
            div.classList.add('activity-loss');
        }

        feed.appendChild(div);
    });
}

// =============================
// QUICK STATS
// =============================
function updateQuickStats(players) {
    if (!players || !players.length) return;

    const totalEl = document.getElementById("total-players");
    const topEl = document.getElementById("top-score");
    const avgEl = document.getElementById("avg-score");

    if (totalEl) totalEl.innerText = players.length;
    if (topEl) topEl.innerText = players[0].score;

    const avg = players.reduce((s,p)=> s+p.score,0)/players.length;
    if (avgEl) avgEl.innerText = Math.round(avg);

    console.log("Stats updated - Total:", players.length, "Top:", players[0].score);
}

// =============================
// ADD PLAYER
// =============================
async function addNewPlayer(){
    const usernameInput = document.getElementById("player-name");
    const scoreInput = document.getElementById("initial-score");
    const username = usernameInput?.value.trim();
    const score = parseInt(scoreInput?.value, 10);

    if(!username || isNaN(score)){
        alert("Enter valid player name and score");
        return;
    }

    try {
        const response = await fetch(`${API_BASE}/addPlayer`, {
            method:"POST",
            headers:{ "Content-Type":"application/json" },
            body:JSON.stringify({ username, score })
        });
        if (!response.ok) throw new Error(`Add player HTTP ${response.status}`);

        console.log("Player added:", username, "score:", score);
        closePlayerModal();
        if (usernameInput) usernameInput.value = "";
        if (scoreInput) scoreInput.value = "1000";
        await fetchDashboard();
        if (document.getElementById('leaderboard-body')) {
            fetchLeaderboard();
        }
    } catch (error) {
        console.error("Add player error:", error);
        useLocalMode = true;

        const local = readLocalState();
        const newPlayer = {
            id: local.nextId,
            username,
            score
        };
        local.nextId += 1;
        local.players.push(newPlayer);
        local.activity.unshift(`Player ${username} joined the game!`);
        if (local.activity.length > 50) {
            local.activity = local.activity.slice(0, 50);
        }
        writeLocalState(local);

        closePlayerModal();
        if (usernameInput) usernameInput.value = "";
        if (scoreInput) scoreInput.value = "1000";

        await fetchDashboard();
        if (document.getElementById('leaderboard-body')) {
            fetchLeaderboard();
        }
    }
}

// =============================
// SIMULATE UPDATE
// =============================
async function simulateServerUpdate() {
    try {
        const response = await fetch(`${API_BASE}/simulate`, {
            method: "POST",
            headers: { "Content-Type": "application/json" }
        });

        if (!response.ok) throw new Error(`Simulate HTTP ${response.status}`);
        console.log('Simulate update triggered');
        await fetchDashboard();
        if (document.getElementById('leaderboard-body')) {
            fetchLeaderboard();
        }
    } catch (error) {
        console.error('Simulate update error:', error);
        useLocalMode = true;
        const local = readLocalState();

        if (local.players.length) {
            const idx = Math.floor(Math.random() * local.players.length);
            const change = Math.floor(Math.random() * 101) - 50;
            local.players[idx].score += change;
            if (local.players[idx].score < 0) local.players[idx].score = 0;

            local.activity.unshift(`Player ${local.players[idx].username} score updated!`);
            if (local.activity.length > 50) {
                local.activity = local.activity.slice(0, 50);
            }
            writeLocalState(local);
        }

        await fetchDashboard();
        if (document.getElementById('leaderboard-body')) {
            fetchLeaderboard();
        }
    }
}

// =============================
// UPDATE SCORE
// =============================
async function updatePlayerScore(id,change){
    try {
        await fetch(`${API_BASE}/api/update`,{
            method:"POST",
            headers:{ "Content-Type":"application/json" },
            body:JSON.stringify({
                player_id:id,
                score_change:change
            })
        });

        console.log("Score updated for player", id, "change:", change);
        fetchLeaderboard();
    } catch (error) {
        console.error("Score update error:", error);
        useLocalMode = true;
        const local = readLocalState();
        const player = local.players.find(p => p.id === id);
        if (player) {
            player.score += change;
            if (player.score < 0) player.score = 0;
            local.activity.unshift(`Player ${player.username} score updated!`);
            if (local.activity.length > 50) {
                local.activity = local.activity.slice(0, 50);
            }
            writeLocalState(local);
            fetchLeaderboard();
            fetchDashboard();
        }
    }
}

// =============================
// SEARCH
// =============================
function filterLeaderboard(text) {
    const filtered = leaderboardData.filter(p => p.username.toLowerCase().includes(text.toLowerCase()));
    updateLeaderboardDisplay(filtered);
}

// =============================
// EVENTS
// =============================
function setupEventListeners(){
    document.getElementById("refresh-btn")?.addEventListener("click", () => {
        if (isDashboardPage()) {
            fetchDashboard();
        } else {
            fetchLeaderboard();
        }
    });

    document.getElementById("add-player-btn")?.addEventListener("click", openPlayerModal);
    document.getElementById("simulate-update-btn")?.addEventListener("click", simulateServerUpdate);

    document.querySelector('.modal .close')?.addEventListener('click', closePlayerModal);
    document.getElementById('player-modal')?.addEventListener('click', (e) => {
        if (e.target === document.getElementById('player-modal')) {
            closePlayerModal();
        }
    });

    document.getElementById("add-player-form")?.addEventListener("submit", (e) => {
        e.preventDefault();
        addNewPlayer();
    });

    document.getElementById("modal-add-player-btn")?.addEventListener("click", (e) => {
        e.preventDefault();
        addNewPlayer();
    });

    document.getElementById("search-player")?.addEventListener("input",(e)=>{
        filterLeaderboard(e.target.value);
    });
}

// =============================
// INIT
// =============================
document.addEventListener("DOMContentLoaded", () => {
    initNavigation();
    setupEventListeners();

    if (updateInterval) {
        clearInterval(updateInterval);
    }
    if (dashboardInterval) {
        clearInterval(dashboardInterval);
    }
    if (activityInterval) {
        clearInterval(activityInterval);
    }

    if (isDashboardPage()) {
        fetchDashboard();
        dashboardInterval = setInterval(fetchDashboard, 10000);
        activityInterval = setInterval(fetchRecentActivity, 5000);
        console.log("Dashboard auto-refresh started (every 10 seconds)");
    } else {
        fetchLeaderboard();
        fetchRecentActivity();
        if (document.getElementById("leaderboard-body")) {
            updateInterval = setInterval(() => {
                fetchLeaderboard();
            }, 12000);
            console.log("Leaderboard auto-refresh started (every 12 seconds)");
        }
    }
});
