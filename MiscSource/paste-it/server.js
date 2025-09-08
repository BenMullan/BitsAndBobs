const express = require('express');
const http = require('http');
const socketIo = require('socket.io');
const fs = require('fs').promises;
const path = require('path');

const app = express();
const server = http.createServer(app);
const io = socketIo(server, {
  cors: {
    origin: "*",
    methods: ["GET", "POST"]
  }
});

const PORT = process.env.PORT || 3000;
const BUCKETS_DIR = path.join(__dirname, 'buckets');
const MAX_IMAGE_SIZE = 5 * 1024 * 1024; // 5MB limit

// Middleware
app.use(express.json({ limit: '10mb' }));
app.use(express.static('public'));

// Initialize buckets directory
async function initializeBuckets() {
  try {
    await fs.access(BUCKETS_DIR);
  } catch {
    await fs.mkdir(BUCKETS_DIR, { recursive: true });
  }

  // Create default bucket if none exist
  const files = await fs.readdir(BUCKETS_DIR);
  const bucketFiles = files.filter(f => f.startsWith('bucket-') && f.endsWith('.json'));
  
  if (bucketFiles.length === 0) {
    const defaultBucket = {
      id: 'bucket-001',
      name: 'General',
      items: [
        {
          id: Date.now().toString(),
          type: 'text',
          content: 'Welcome to your paste sharing service! Paste anything here with Ctrl+V.',
          timestamp: new Date().toISOString()
        }
      ]
    };
    await fs.writeFile(
      path.join(BUCKETS_DIR, 'bucket-001.json'),
      JSON.stringify(defaultBucket, null, 2)
    );
  }
}

// Helper functions
async function getBucketList() {
  const files = await fs.readdir(BUCKETS_DIR);
  const bucketFiles = files.filter(f => f.startsWith('bucket-') && f.endsWith('.json'));
  
  const buckets = [];
  for (const file of bucketFiles.sort()) {
    try {
      const content = await fs.readFile(path.join(BUCKETS_DIR, file), 'utf8');
      const bucket = JSON.parse(content);
      buckets.push({
        id: bucket.id,
        name: bucket.name || bucket.id,
        itemCount: bucket.items?.length || 0
      });
    } catch (error) {
      console.error(`Error reading bucket ${file}:`, error);
    }
  }
  
  return buckets;
}

async function getBucket(bucketId) {
  try {
    const content = await fs.readFile(path.join(BUCKETS_DIR, `${bucketId}.json`), 'utf8');
    return JSON.parse(content);
  } catch (error) {
    return null;
  }
}

async function saveBucket(bucket) {
  await fs.writeFile(
    path.join(BUCKETS_DIR, `${bucket.id}.json`),
    JSON.stringify(bucket, null, 2)
  );
}

async function getNextBucketId() {
  const buckets = await getBucketList();
  const maxNum = buckets.reduce((max, bucket) => {
    const match = bucket.id.match(/bucket-(\d+)/);
    return match ? Math.max(max, parseInt(match[1])) : max;
  }, 0);
  return `bucket-${String(maxNum + 1).padStart(3, '0')}`;
}

// API Routes
app.get('/api/buckets', async (req, res) => {
  try {
    const buckets = await getBucketList();
    res.json(buckets);
  } catch (error) {
    console.error('Error fetching buckets:', error);
    res.status(500).json({ error: 'Failed to fetch buckets' });
  }
});

app.get('/api/buckets/:bucketId', async (req, res) => {
  try {
    const bucket = await getBucket(req.params.bucketId);
    if (!bucket) {
      return res.status(404).json({ error: 'Bucket not found' });
    }
    res.json(bucket);
  } catch (error) {
    console.error('Error fetching bucket:', error);
    res.status(500).json({ error: 'Failed to fetch bucket' });
  }
});

app.post('/api/buckets', async (req, res) => {
  try {
    const { name } = req.body;
    if (!name || typeof name !== 'string' || name.trim().length === 0) {
      return res.status(400).json({ error: 'Valid bucket name required' });
    }

    const bucketId = await getNextBucketId();
    const newBucket = {
      id: bucketId,
      name: name.trim(),
      items: []
    };

    await saveBucket(newBucket);
    
    // Notify all clients
    const buckets = await getBucketList();
    io.emit('bucketsUpdated', buckets);
    
    res.json(newBucket);
  } catch (error) {
    console.error('Error creating bucket:', error);
    res.status(500).json({ error: 'Failed to create bucket' });
  }
});

app.post('/api/buckets/:bucketId/items', async (req, res) => {
  try {
    const { type, content } = req.body;
    
    if (!type || !content) {
      return res.status(400).json({ error: 'Type and content required' });
    }

    if (type === 'image' && content.length > MAX_IMAGE_SIZE) {
      return res.status(413).json({ error: 'Image too large (max 5MB)' });
    }

    const bucket = await getBucket(req.params.bucketId);
    if (!bucket) {
      return res.status(404).json({ error: 'Bucket not found' });
    }

    const newItem = {
      id: Date.now().toString() + Math.random().toString(36).substr(2, 9),
      type,
      content,
      timestamp: new Date().toISOString()
    };

    bucket.items.unshift(newItem); // Add to beginning for newest-first order
    await saveBucket(bucket);

    // Notify all clients
    io.emit('bucketUpdated', bucket);

    res.json(newItem);
  } catch (error) {
    console.error('Error adding item:', error);
    res.status(500).json({ error: 'Failed to add item' });
  }
});

app.delete('/api/buckets/:bucketId/items/:itemId', async (req, res) => {
  try {
    const bucket = await getBucket(req.params.bucketId);
    if (!bucket) {
      return res.status(404).json({ error: 'Bucket not found' });
    }

    bucket.items = bucket.items.filter(item => item.id !== req.params.itemId);
    await saveBucket(bucket);

    // Notify all clients
    io.emit('bucketUpdated', bucket);

    res.json({ success: true });
  } catch (error) {
    console.error('Error deleting item:', error);
    res.status(500).json({ error: 'Failed to delete item' });
  }
});

// WebSocket handling
io.on('connection', (socket) => {
  console.log('Client connected');

  socket.on('disconnect', () => {
    console.log('Client disconnected');
  });
});

// Initialize and start server
async function startServer() {
  try {
    await initializeBuckets();
    server.listen(PORT, () => {
      console.log(`Paste sharing service running on port ${PORT}`);
      console.log(`Open http://localhost:${PORT} in your browser`);
    });
  } catch (error) {
    console.error('Failed to start server:', error);
    process.exit(1);
  }
}

startServer();