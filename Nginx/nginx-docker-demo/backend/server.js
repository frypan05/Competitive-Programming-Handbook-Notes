
const http = require('http');

const server = http.createServer((req, res) => {
    if (req.url === '/api') {
        res.writeHead(200, { 'Content-Type': 'application/json' });
        return res.end(JSON.stringify({ message: 'Hello from the backend' }));
    }
    res.writeHead(404, { 'Content-Type': 'text/plain' });
    res.end('Not Found');
})

server.listen(3000, () => {
    console.log("Backend Running on port 3000");
});
