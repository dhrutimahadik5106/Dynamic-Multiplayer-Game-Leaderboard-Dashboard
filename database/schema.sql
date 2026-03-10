/*-------------------------------------------------
  Create Database
-------------------------------------------------*/

CREATE DATABASE IF NOT EXISTS leaderboard_db;

USE leaderboard_db;


/*-------------------------------------------------
  Players Table
-------------------------------------------------*/

CREATE TABLE players (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(100) NOT NULL UNIQUE,
    score INT DEFAULT 0,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);


/*-------------------------------------------------
  Activity Feed Table
-------------------------------------------------*/

CREATE TABLE activities (
    id INT AUTO_INCREMENT PRIMARY KEY,
    message TEXT NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);


/*-------------------------------------------------
  Score History Table
-------------------------------------------------*/

CREATE TABLE score_history (
    id INT AUTO_INCREMENT PRIMARY KEY,
    player_name VARCHAR(100),
    score_change INT,
    new_score INT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
