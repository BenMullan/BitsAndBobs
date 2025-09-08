# paste-it
_GPT-4 (+ bits from Ben Mullan), 2025_
<br/><br/>
A real-time shared webpage-accessible clipboard
- run with: `npm i` then `node server.js`
- use port-tunneling to make publicly-accessible


### Supported Content Types

- **Text**: Any plain text content pasted via clipboard
- **Images**: PNG, JPEG, GIF, and other image formats (up to 5MB)

### Keyboard Shortcuts

- `Ctrl+V` - Paste clipboard content
- `Escape` - Close modal dialogs
- `Enter` - Confirm actions in modal dialogs

## Project Structure

```
paste-sharing-service/
├── server.js              # Main server application
├── package.json           # Dependencies and scripts
├── public/
│   └── index.html         # Frontend application
└── buckets/               # Auto-created bucket storage directory
    ├── bucket-001.json    # Individual bucket files
    ├── bucket-002.json
    └── ...
```

## API Endpoints

### Buckets
- `GET /api/buckets` - List all buckets
- `POST /api/buckets` - Create a new bucket
- `GET /api/buckets/:bucketId` - Get specific bucket with items

### Items
- `POST /api/buckets/:bucketId/items` - Add new item to bucket
- `DELETE /api/buckets/:bucketId/items/:itemId` - Delete item from bucket

### WebSocket Events
- `bucketsUpdated` - Broadcast when bucket list changes
- `bucketUpdated` - Broadcast when bucket content changes

## Configuration

### Environment Variables

- `PORT` - Server port (default: 3000)

### Limits

- Maximum image size: 5MB per image
- Request body limit: 10MB
- No authentication (suitable for trusted networks)