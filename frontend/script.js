// =============================
// GLOBAL VARIABLES
// =============================

let leaderboardData = [];
let updateInterval;

const API_BASE = "http://localhost:8080/api";

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
// INITIALIZE
// =============================

document.addEventListener("DOMContentLoaded", function () {

    fetchLeaderboard();
    fetchRecentActivity();

    startAutoRefresh();
    setupEventListeners();
});


// =============================
// EVENT LISTENERS
// =============================

function setupEventListeners() {

    const addBtn = document.getElementById("add-player-btn");
    const simulateBtn = document.getElementById("simulate-update-btn");
    const refreshBtn = document.getElementById("refresh-btn");
    const searchInput = document.getElementById("search-player");

    if(addBtn)
        addBtn.addEventListener("click", () => {
            document.getElementById("player-modal").style.display = "block";
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
// FETCH ACTIVITY
// =============================

async function fetchRecentActivity() {

    try {

        const response = await fetch(`${API_BASE}/activity`);
        const data = await response.json();

        data.forEach(item=>{
            enqueueMatchHistory(item);
        });

        updateActivityFeed(matchHistoryQueue);

    } catch (error) {

        console.error("Activity fetch error:", error);
    }
}


// =============================
// UPDATE LEADERBOARD TABLE
// =============================

function updateLeaderboardDisplay(players) {

    const tbody = document.getElementById("leaderboard-body");

    if(!tbody) return;

    tbody.innerHTML = "";

    players.forEach((player, index) => {

        const row = document.createElement("tr");

        let rankDisplay = "#" + (index+1);

        if(index === 0) rankDisplay = "🥇";
        else if(index === 1) rankDisplay = "🥈";
        else if(index === 2) rankDisplay = "🥉";

        if(index===0) row.classList.add("rank-1");
        if(index===1) row.classList.add("rank-2");
        if(index===2) row.classList.add("rank-3");

        row.innerHTML = `
        <td>${rankDisplay}</td>
        <td>${player.username}</td>
        <td class="score-cell">${player.score}</td>
        <td class="status-online">${player.status || "online"}</td>
        <td>
            <button class="btn-score" onclick="updatePlayerScore(${player.id},10)">+10</button>
            <button class="btn-score" onclick="updatePlayerScore(${player.id},-5)">-5</button>
        </td>
        `;

        // smooth row animation
        row.style.transition = "transform 0.3s ease";

        tbody.appendChild(row);
    });

    updateTopPlayers(players.slice(0,5));
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

        await fetch(`${API_BASE}/player`,{

            method:"POST",

            headers:{
                "Content-Type":"application/json"
            },

            body:JSON.stringify({
                username:username,
                score:score
            })
        });

        // close modal
        document.getElementById("player-modal").style.display = "none";

        // clear input fields
        document.getElementById("player-name").value = "";
        document.getElementById("initial-score").value = "";

        // refresh everything
        await refreshDashboard();

        // optional popup
        showScorePopup("+ Player Added");

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
// SIMULATE RANDOM UPDATE
// =============================

async function simulateRandomUpdate(){

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

    },3000);
}
