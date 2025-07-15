const mysql = require('mysql2/promise');
const bcrypt = require('bcrypt'); // ✅ Add bcrypt for hashing

// Create DB connection
async function connectDB() {
  return await mysql.createConnection({
    host: 'phpmyadmin.romangry.fr',
    user: 'fp_1_5',
    password: 'PNPhStud3nt_1_5',
    database: 'fp_1_5'
  });
}

async function main() {
  const [accountId, username, password, score, wins, losses] = process.argv.slice(2);

  if (!accountId || !username || !password || score === undefined || wins === undefined || losses === undefined) {
    console.error('❌ Missing arguments.\nUsage: node sendStats.js <accountId> <username> <password> <score> <wins> <losses>');
    process.exit(1);
  }

  try {
    const db = await connectDB();

    // 🔒 Hash password only if not already hashed
    const hashedPassword = password.startsWith('$2b$')
      ? password
      : await bcrypt.hash(password, 10);

    // ✅ Insert or update account info with hashed password
    await db.execute(`
      INSERT INTO accounts (account_id, username, password)
      VALUES (?, ?, ?)
      ON DUPLICATE KEY UPDATE username = VALUES(username), password = VALUES(password)
    `, [accountId, username, hashedPassword]);

    // ✅ Insert or update stats
    await db.execute(`
      INSERT INTO stats (account_id, score, wins, losses)
      VALUES (?, ?, ?, ?)
      ON DUPLICATE KEY UPDATE score = VALUES(score), wins = VALUES(wins), losses = VALUES(losses)
    `, [accountId, score, wins, losses]);

    console.log('✅ Stats successfully synced to database.');
    await db.end();
  } catch (err) {
    console.error('❌ Database error:', err.message);
    process.exit(1);
  }
}

main();
