const mysql = require('mysql2/promise'); // ✅ use promise version

const pool = mysql.createPool({
  host: 'localhost',
  user: 'klint',
  password: 'klintismypassword',
  database: 'klint',
  waitForConnections: true,
  connectionLimit: 10,
  queueLimit: 0
});

module.exports = pool;
