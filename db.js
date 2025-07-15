const mysql = require('mysql2/promise'); // ✅ use promise version

const pool = mysql.createPool({
  host: 'phpmyadmin.romangry.fr',
  user: 'fp_1_5',
  password: 'PNPhStud3nt_1_5',
  database: 'fp_1_5',
  waitForConnections: true,
  connectionLimit: 10,
  queueLimit: 0
});

module.exports = pool;
