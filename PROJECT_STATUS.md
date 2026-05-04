# Dynamic Multiplayer Game Leaderboard Dashboard - Project Status

## Project Overview
This project simulates a real-time leaderboard system for an online multiplayer game or tournament. Players can earn or lose points dynamically, join or leave the game at any time, and the system updates rankings and activity logs in real time. The project uses multiple advanced data structures to ensure fast performance.

## Architecture
- **Backend**: C++ HTTP server using Winsock library
- **Frontend**: HTML/CSS/JavaScript web dashboard
- **Data Structures**: AVL Tree, Max Heap, Activity List, Event Queue
- **Database**: SQL schema provided (though server currently uses file-based storage)

## Backend Components
- `server.cpp`: Main HTTP server handling API endpoints and serving frontend files
- `leaderboard.cpp`: Core leaderboard logic with player management
- `avl_tree.cpp`: AVL tree implementation for sorted player storage
- `heap.cpp`: Max heap for efficient top-player retrieval
- `activity_list.cpp`: Doubly-linked list for activity logs
- `event_queue.cpp`: Queue for event management
- `player.h`: Player struct definition

## Frontend Components
- `index.html`: Main dashboard page
- `leaderboard.html`: Leaderboard view
- `network.html`: Player network visualization
- `activity.html`: Activity feed
- `datastructures.html`: Data structures visualization
- `stats.html`: Statistics page
- Various JS files for functionality
- `style.css`: Styling

## API Endpoints
- GET `/leaderboard`: Get current leaderboard
- GET `/topPlayers`: Get top players
- GET `/activity`: Get activity feed
- GET `/stats`: Get statistics
- GET `/api/network`: Get player network data
- POST `/addPlayer`: Add new player
- POST `/simulate`: Simulate random updates
- POST `/api/update`: Update player score

## Recent Changes
- Added missing `#include "player.h"` to `avl_tree.cpp` and `heap.cpp`
- Removed duplicate `Player` struct definition from `leaderboard.cpp` and replaced with include
- Fixed compilation issues for Winsock linking (-lws2_32)

## How to Run
1. Compile the server: `g++ backend/server.cpp backend/leaderboard.cpp backend/activity_list.cpp backend/avl_tree.cpp backend/event_queue.cpp backend/heap.cpp -o backend/server -lws2_32`
2. Run the server: `backend/server.exe`
3. Open browser to `http://localhost:8080`

## Current Status
- Backend server is functional and serves both API and static frontend files
- Frontend dashboard is implemented with multiple views
- Real-time updates via API calls
- Data structures are implemented and integrated
- Project is ready to run with the above steps

## Files Modified Recently
- `backend/avl_tree.cpp`: Added player.h include
- `backend/heap.cpp`: Added player.h include  
- `backend/leaderboard.cpp`: Removed duplicate Player struct, added include

## Known Issues
- None currently identified
- Server uses file-based storage (players.txt) rather than SQL database
- Frontend assumes localhost:8080 for API calls