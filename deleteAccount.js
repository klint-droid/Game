const mysql = require('mysql2/promise');

// Connect to MySQL
async function connectDB() {
  return await mysql.createConnection({
    host: 'phpmyadmin.romangry.fr',
    user: 'fp_1_5',
    password: 'PNPhStud3nt_1_5',
    database: 'fp_1_5'
  });
}

async function main() {
  const accountId = process.argv[2];

if (!accountId) {
  console.error('❌ Missing accountId.\nUsage: node deleteAccount.js <accountId>');
  process.exit(1);
}

  try {
    const db = await connectDB();

    // First delete from stats (due to foreign key constraint if any)
    await db.execute(`DELETE FROM stats WHERE account_id = ?`, [accountId]);

    // Then delete from accounts
    const [result] = await db.execute(`DELETE FROM accounts WHERE account_id = ?`, [accountId]);

    if (result.affectedRows === 0) {
      console.log(`No account found with ID ${accountId}.`);
    } else {
      console.log(`Account ${accountId} and related stats deleted successfully.`);
    }

    await db.end();
  } catch (err) {
    console.error('❌ Database error:', err.message);
    process.exit(1);
  }
}

main();
