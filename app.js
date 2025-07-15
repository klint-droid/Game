const express = require('express');
const http = require('http');
const morgan = require('morgan');
const socketIo = require('socket.io');
const db = require('./db');
const path = require('path');
const session = require('express-session');
const bcrypt = require('bcrypt');

const app = express();
const server = http.createServer(app);
const io = socketIo(server);
const PORT = 3000;

// View engine & middlewares
app.set('view engine', 'ejs');
app.set('views', path.join(__dirname, 'views'));
app.use(morgan('dev'));
app.use(express.static('public'));
app.use(express.urlencoded({ extended: true }));
app.use(express.json());

// Session config
app.use(session({
  secret: 'klint-is-the-key',
  resave: false,
  saveUninitialized: false
}));

// 🔐 Middleware to protect routes
function requireLogin(req, res, next) {
  if (req.session && req.session.user) return next();
  return res.redirect('/login');
}

// 🔑 Login routes
app.get('/login', (req, res) => {
  res.render('login'); // Make sure views/login.ejs exists
});

app.post('/login', async (req, res) => {
  const { username, password } = req.body;
  try {
    const [rows] = await db.query(
      'SELECT * FROM accounts WHERE username = ?',
      [username]
    );

    const user = rows[0];
    if (user && await bcrypt.compare(password, user.password)) {
      req.session.user = {
        id: user.account_id,
        username: user.username
      };
      return res.redirect('/');
    } else {
      return res.status(401).send('Invalid username or password');
    }
  } catch (err) {
    console.error('Login error:', err);
    res.status(500).send('Server error');
  }
});

app.get('/logout', (req, res) => {
  req.session.destroy(() => {
    res.redirect('/login');
  });
});

// 🏠 Protected Routes
app.get('/', requireLogin, (req, res) => {
  res.render('index', {active: 'home'});
});

app.get('/leaderboard', requireLogin, (req, res) => {
  res.render('leaderboard', {active: 'leaderboard'});
});

// 📊 Real-time leaderboard 
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
    console.error('Leaderboard error:', err);
  }
};

(async () => {
  await emitLeaderboard();
})();

app.get('/sync', async (req, res) => {
  await emitLeaderboard();
  res.send('Leaderboard refreshed.');
});

io.on('connection', (socket) => {
  console.log('New client connected');
  emitLeaderboard();
});

server.listen(PORT, () => {
  console.log(`Server running at http://localhost:${PORT}`);
});
