const express = require('express');
const http = require('http');
const morgan = require('morgan');
const socketIo = require('socket.io');
const db = require('./db');
const path = require('path');

const app = express();
const server = http.createServer(app);
const io = socketIo(server);
const PORT = 3000;

app.set('view engine', 'ejs');
app.set('views', path.join(__dirname, 'views'));

app.use(morgan('dev'));
app.use(express.static('public'));

// ✅ Route to index.ejs (landing page)
app.get('/', (req, res) => {
  res.render('index'); // Make sure views/index.ejs exists
});

// ✅ Route to leaderboard.ejs
app.get('/leaderboard', (req, res) => {
  res.render('leaderboard'); // Make sure views/leaderboard.ejs exists
});

// 🔁 Real-time leaderboard emitter
const emitLeaderboard = async () => {
  try {
    const [rows] = await db.query(`
      SELECT a.username, s.score, s.wins, s.losses
      FROM stats s
      JOIN accounts a ON s.account_id = a.account_id
      ORDER BY s.score DESC
    `);
    io.emit('leaderboard', rows);
  } catch (err) {
    console.error('❌ Leaderboard error:', err);
  }
};

// Emit on startup
(async () => {
  await emitLeaderboard();
})();

// 🔄 Manual refresh via browser
app.get('/sync', async (req, res) => {
  await emitLeaderboard();
  res.send('✅ Leaderboard refreshed.');
});

// Socket.IO connection
io.on('connection', (socket) => {
  console.log('👥 New client connected');
  emitLeaderboard();
});

server.listen(PORT, () => {
  console.log(`🚀 Server running at http://localhost:${PORT}`);
});
