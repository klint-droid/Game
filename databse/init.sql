-- Create database if not exists
CREATE DATABASE IF NOT EXISTS klint;
USE klint;

-- Table: accounts
CREATE TABLE IF NOT EXISTS accounts (
    account_id VARCHAR(15) PRIMARY KEY,
    username VARCHAR(50) NOT NULL,
    password VARCHAR(50) NOT NULL
);

-- Table: stats
CREATE TABLE IF NOT EXISTS stats (
    account_id VARCHAR(15) PRIMARY KEY,
    score INT DEFAULT 0,
    wins INT DEFAULT 0,
    losses INT DEFAULT 0,
    FOREIGN KEY (account_id) REFERENCES accounts(account_id) ON DELETE CASCADE
);
