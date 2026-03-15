// =============================
// NAVIGATION SYSTEM
// =============================

function initNavigation() {
    // Set active navigation link based on current page
    const currentPath = window.location.pathname;
    const currentPage = currentPath.split('/').pop().replace('.html', '') || 'index';

    const navLinks = document.querySelectorAll('.nav-link');
    navLinks.forEach(link => {
        const href = link.getAttribute('href');
        const linkPage = href.replace('.html', '');

        if (linkPage === currentPage || (currentPage === 'index' && linkPage === 'index.html')) {
            link.classList.add('active');
        }
    });
}

function navigateToPage(pageName) {
    // For multi-page application, we use regular browser navigation
    if (pageName === 'index') {
        window.location.href = 'index.html';
    } else {
        window.location.href = `${pageName}.html`;
    }
}

// =============================
// GLOBAL VARIABLES
// =============================

let leaderboardData = [];
let activityData = [];
let statsData = {};
let updateInterval;

const API_BASE = "http://localhost:8080";

// =============================
// MATCH HISTORY QUEUE
// =============================

let matchHistoryQueue = [];
const MAX_HISTORY = 20;

function enqueueMatchHistory(activity){

    matchHistoryQueue.push(activity);

    if(matchHistoryQueue.length > MAX_HISTORY){
        matchHistoryQueue.shift(); // dequeue oldest
    }
}

async function refreshDashboard(){

    await fetchLeaderboard();
    await fetchRecentActivity();

}


// =============================
// MODAL HELPERS
// =============================

function openModal() {
    const modal = document.getElementById("player-modal");
    if (!modal) return;
    modal.classList.add("open");
}

function closeModal() {
    const modal = document.getElementById("player-modal");
    if (!modal) return;
    modal.classList.remove("open");
}


// =============================
// LOAD FUNCTIONS
// =============================

function loadLeaderboard() {
    fetchLeaderboard();
}

function loadStats() {
    updateQuickStats(leaderboardData);
}


// =============================
// INITIALIZE
// =============================

document.addEventListener("DOMContentLoaded", function () {

    fetchLeaderboard();
    fetchRecentActivity();

    startAutoRefresh();
    setupEventListeners();

    // Auto-refresh leaderboard and stats every 30 seconds
    setInterval(() => {
        loadLeaderboard();
        loadStats();
    }, 30000);
});


// =============================
// EVENT LISTENERS
// =============================

function setupEventListeners() {

    const addBtn = document.getElementById("add-player-btn");
    const modalAddBtn = document.getElementById("modal-add-player-btn");
    const simulateBtn = document.getElementById("simulate-update-btn");
    const refreshBtn = document.getElementById("refresh-btn");
    const searchInput = document.getElementById("search-player");
    const addForm = document.getElementById("add-player-form");
    const closeBtn = document.getElementById("player-modal").querySelector(".close");

    if(addBtn)
        addBtn.addEventListener("click", () => {
            openModal();
        });

    if(addForm)
        addForm.addEventListener("submit", function(e) {
            e.preventDefault();
            addNewPlayer();
        });

    if(modalAddBtn)
        modalAddBtn.addEventListener("click", () => {
            addNewPlayer();
        });

    if(closeBtn)
        closeBtn.addEventListener("click", () => {
            closeModal();
        });

    if(simulateBtn)
        simulateBtn.addEventListener("click", simulateRandomUpdate);

    if(refreshBtn)
        refreshBtn.addEventListener("click", () => {
            fetchLeaderboard();
            fetchRecentActivity();
        });

    if(searchInput)
        searchInput.addEventListener("input", function(e){
            filterLeaderboard(e.target.value);
        });

}


// =============================
// FETCH LEADERBOARD
// =============================

async function fetchLeaderboard() {

    try {

        const response = await fetch(`${API_BASE}/leaderboard`);
        const data = await response.json();

        leaderboardData = data;

        updateLeaderboardDisplay(data);
        updateQuickStats(data);

    } catch (error) {

        console.error("Leaderboard fetch error:", error);
    }
}


// =============================
// FETCH STATS
// =============================

async function fetchStats() {

    try {

        const response = await fetch(`${API_BASE}/stats`);
        const data = await response.json();

        statsData = data.stats;

        updateStatsDisplay(data.stats);

    } catch (error) {

        console.error("Stats fetch error:", error);
    }
}


// =============================
// UPDATE LEADERBOARD TABLE
// =============================

function updateLeaderboardDisplay(players) {

    const tbody = document.getElementById("leaderboard-body");

    if(!tbody) return;

    // Sort players by score descending
    players.sort((a, b) => b.score - a.score);

    // Assume fixed row height for animation
    const rowHeight = 60; // approximate height in px

    // Get current rows
    const currentRows = Array.from(tbody.children);

    // Map player.id to existing row
    const rowMap = {};
    currentRows.forEach(row => {
        const id = row.id.replace('player-', '');
        rowMap[id] = row;
    });

    // Create or update rows in new order
    const newOrder = [];
    players.forEach((player, index) => {
        let row = rowMap[player.id];
        if (!row) {
            row = document.createElement("tr");
            row.className = "player-row";
            row.id = `player-${player.id}`;
            row.style.transition = "transform 0.5s ease";
            tbody.appendChild(row); // append to end initially
        }

        // Update content
        let rankDisplay = "#" + (index + 1);
        if (index === 0) rankDisplay = "🥇";
        else if (index === 1) rankDisplay = "🥈";
        else if (index === 2) rankDisplay = "🥉";

        row.className = "player-row";
        if (index === 0) row.classList.add("rank-1");
        if (index === 1) row.classList.add("rank-2");
        if (index === 2) row.classList.add("rank-3");

        row.innerHTML = `
        <td>${rankDisplay}</td>
        <td>${player.username}</td>
        <td class="score-cell">${player.score}</td>
        <td class="change-cell">
            <span class="change-neutral">—</span>
        </td>
        <td class="status-online">${player.status || "online"}</td>
        <td>
            <button class="btn-score" onclick="updatePlayerScore(${player.id},10)">+10</button>
            <button class="btn-score" onclick="updatePlayerScore(${player.id},-5)">-5</button>
        </td>
        `;

        newOrder.push(row);
    });

    // Calculate current positions and set transforms for animation
    newOrder.forEach((row, newIndex) => {
        const currentIndex = Array.from(tbody.children).indexOf(row);
        const deltaY = (currentIndex - newIndex) * rowHeight;
        row.style.transform = `translateY(${deltaY}px)`;
    });

    // After animation, reorder DOM and reset transforms
    setTimeout(() => {
        tbody.innerHTML = "";
        newOrder.forEach(row => {
            row.style.transform = "";
            tbody.appendChild(row);
        });
    }, 500); // match transition duration

    updateTopPlayers(players.slice(0, 5));
}


// =============================
// TOP PLAYERS PANEL
// =============================

function updateTopPlayers(players) {

    const container = document.getElementById("top-players");

    if(!container) return;

    container.innerHTML = "";

    players.forEach((p,index)=>{

        let badge = "#" + (index+1);

        if(index === 0) badge = "🥇";
        else if(index === 1) badge = "🥈";
        else if(index === 2) badge = "🥉";

        const card = document.createElement("div");

        card.className = "player-card";

        card.innerHTML = `
        <div class="rank-badge">${badge}</div>
        <div class="player-info">
            <div class="player-name">${p.username}</div>
            <div class="player-score">${p.score} pts</div>
        </div>
        `;

        container.appendChild(card);
    });
}


// =============================
// ACTIVITY FEED
// =============================
function updateActivityFeed(data) {

    const feed = document.getElementById("activity-feed");

    if(!feed) return;

    feed.innerHTML = "";

    data.slice().reverse().forEach(item=>{

        const div = document.createElement("div");

        div.className = "activity-item";

        div.innerText = item;

        feed.appendChild(div);
    });
}

// =============================
// QUICK STATS
// =============================

function updateQuickStats(players) {

    if(players.length === 0) return;

    document.getElementById("total-players").innerText = players.length;
    document.getElementById("top-score").innerText = players[0].score;

    const avg = players.reduce((s,p)=> s+p.score,0)/players.length;

    document.getElementById("avg-score").innerText = Math.round(avg);
}


// =============================
// ADD PLAYER
// =============================
async function addNewPlayer(){

    const username = document.getElementById("player-name").value;
    const score = parseInt(document.getElementById("initial-score").value);

    if(!username || isNaN(score)){
        alert("Enter valid player name and score");
        return;
    }

    try{

        const response = await fetch(`${API_BASE}/addPlayer`, {

            method:"POST",

            headers:{
                "Content-Type":"application/json"
            },

            body:JSON.stringify({
                username:username,
                score:score
            })
        });

        if (!response.ok) {
            console.error("Failed to add player", response.status);
            return;
        }

        // close modal with transition
        closeModal();

        // clear input fields
        document.getElementById("player-name").value = "";
        document.getElementById("initial-score").value = "";

        // refresh everything
        await refreshDashboard();

        // success toast
        showScorePopup("Player added successfully");

    }catch(error){

        console.error("Add player error:",error);
    }
}

// =============================
// UPDATE SCORE
// =============================

async function updatePlayerScore(playerId,change){

    try{

        await fetch(`${API_BASE}/update`,{

            method:"POST",

            headers:{
                "Content-Type":"application/json"
            },

            body:JSON.stringify({
                player_id:playerId,
                score_change:change
            })
        });

        showScorePopup(change);

        fetchLeaderboard();
        fetchRecentActivity();

    }catch(error){

        console.error("Score update error:",error);
    }
}


// =============================
// SCORE POPUP
// =============================

function showScorePopup(change){

    const popup=document.createElement("div");

    popup.className="score-popup";

    popup.innerText=change>0?`+${change} Score`:`${change} Score`;

    document.body.appendChild(popup);

    setTimeout(()=>popup.remove(),1500);
}


// =============================
// FILTER LEADERBOARD
// =============================

function filterLeaderboard(searchTerm) {
    const tbody = document.getElementById("leaderboard-body");
    if (!tbody) return;

    const rows = tbody.querySelectorAll("tr");
    const term = searchTerm.toLowerCase().trim();

    rows.forEach(row => {
        const playerName = row.cells[1]?.textContent.toLowerCase() || "";
        if (term === "" || playerName.includes(term)) {
            row.style.display = "";
        } else {
            row.style.display = "none";
        }
    });
}

    try{

        await fetch(`${API_BASE}/simulate`,{
            method:"POST"
        });

        fetchLeaderboard();
        fetchRecentActivity();

    }catch(error){

        console.error("Simulation error:",error);
    }
}


// =============================
// SEARCH
// =============================

function filterLeaderboard(text){

    const filtered = leaderboardData.filter(p =>
        p.username.toLowerCase().includes(text.toLowerCase())
    );

    updateLeaderboardDisplay(filtered);
}


// =============================
// AUTO REFRESH
// =============================

function startAutoRefresh(){

    updateInterval = setInterval(()=>{

        fetchLeaderboard();
        fetchRecentActivity();

    },10000);
}


// =============================
// ADD PLAYER (RANDOM)
// =============================

async function addPlayer() {

    const randomUsername = "Player" + Math.floor(Math.random() * 10000);
    const randomScore = Math.floor(Math.random() * 1000);

    try {

        const response = await fetch(`${API_BASE}/addPlayer`, {

            method: 'POST',

            headers: {
                'Content-Type': 'application/json'
            },

            body: JSON.stringify({
                username: randomUsername,
                score: randomScore
            })

        });

        if (response.ok) {

            await refreshDashboard();

        } else {

            console.error("Failed to add player");

        }

    } catch (error) {

        console.error("Error adding player:", error);

    }

}


// =============================
// PAGE-AWARE INITIALIZATION
// =============================

function initializePage() {
    const currentPath = window.location.pathname;
    const currentPage = currentPath.split('/').pop().replace('.html', '') || 'index';

    // Initialize navigation on all pages
    initNavigation();

    // Page-specific initialization
    switch(currentPage) {
        case 'index':
            // Dashboard page
            initEventListeners();
            fetchLeaderboard();
            fetchRecentActivity();
            break;

        case 'leaderboard':
            // Leaderboard page
            initLeaderboardPage();
            break;

        case 'network':
            // Network page - handled by network.js
            break;

        case 'activity':
            // Activity page
            initActivityPage();
            break;

        case 'stats':
            // Stats page - handled by stats.js
            break;

        case 'datastructures':
            // Data structures page - handled by datastructures.js
            break;

        default:
            // Default to dashboard functionality
            initEventListeners();
            fetchLeaderboard();
            fetchRecentActivity();
    }
}

// =============================
// PAGE INITIALIZERS
// =============================

function initLeaderboardPage() {
    // Initialize leaderboard-specific elements
    fetchLeaderboard();

    // Set up search functionality
    const searchInput = document.getElementById('search-player');
    if (searchInput) {
        searchInput.addEventListener('input', function(e) {
            filterLeaderboard(e.target.value);
        });
    }

    // Set up refresh button
    const refreshBtn = document.getElementById('refresh-btn');
    if (refreshBtn) {
        refreshBtn.addEventListener('click', () => {
            fetchLeaderboard();
        });
    }

    // Auto-refresh for leaderboard
    updateInterval = setInterval(() => {
        fetchLeaderboard();
    }, 5000);
}

function initActivityPage() {
    // Initialize activity-specific elements
    fetchRecentActivity();

    // Auto-refresh for activity
    updateInterval = setInterval(() => {
        fetchRecentActivity();
    }, 5000);
}

// =============================
// INITIALIZATION
// =============================

document.addEventListener('DOMContentLoaded', function() {
    initializePage();
});

// Cleanup on page unload
window.addEventListener('beforeunload', function() {
    if (updateInterval) {
        clearInterval(updateInterval);
    }
});