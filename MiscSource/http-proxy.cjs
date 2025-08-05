#!/usr/bin/env node

/*
    file:       http-proxy.cjs - intercepts http requests to disk/console
    author:     Ben Mullan 2025
    pre-req:    npm i yargs
    exec:       eg: node http-proxy.js --port 8080 --endpoint https://example.com:9001 --requests-to-disk --requests-to-console --responses-to-disk --responses-to-console
*/


const http    = require("http");
const https   = require("https");
const fs      = require("fs");
const path    = require("path");
const { URL } = require("url");
const yargs   = require("yargs");


// ------ parse CLAs ------
const argv = yargs
    .option("port",                 { type: "number",  default: 8080 })
    .option("endpoint",             { type: "string",  demandOption: true })
    .option("requests-to-disk",     { type: "boolean", default: false })
    .option("requests-to-console",  { type: "boolean", default: false })
    .option("responses-to-disk",    { type: "boolean", default: false })
    .option("responses-to-console", { type: "boolean", default: false })
    .argv
;

const endpointUrl = new URL(argv.endpoint);
const isHttps     = (endpointUrl.protocol === "https:");
const clientLib   = (isHttps ? https : http);


// ------ prepare log directories, if needed ------
if (argv["requests-to-disk"])   { fs.mkdirSync(path.join(__dirname, "logs/requests"), { recursive: true }); }
if (argv["responses-to-disk"])  { fs.mkdirSync(path.join(__dirname, "logs/responses"), { recursive: true }); }


// ------ some helpers ------
function dumpToConsole(prefix, raw) {
    console.log(`\n----- ${prefix} -----\n${raw}\n----------------------`);
}

function dumpToDisk(dir, id, raw) {
    const file = path.join(__dirname, dir, `${Date.now()}-${id}.txt`);
    fs.writeFile(
        file,
        raw,
        err => { if (err) console.error(`Failed to write ${file}:`, err); }
    );
}


// ------ start the proxy server ------
http.createServer(
    (req, res) => {
        
        const id        = Math.random().toString(36).substr(2, 6);
        const chunksReq = [];

        // collect incoming request body
        req.on("data", chunk => chunksReq.push(chunk));
        
        req.on(
            "end",
            () => {
                
                const bodyReq = Buffer.concat(chunksReq).toString();
                
                const rawReq  = [
                    `${req.method} ${req.url} HTTP/${req.httpVersion}`,
                    ...Object.entries(req.headers).map(([k, v]) => `${k}: ${v}`),
                    "",
                    bodyReq
                ].join("\r\n");

                // log request
                if (argv["requests-to-console"])    { dumpToConsole(`REQUEST [${id}]`, rawReq); }
                if (argv["requests-to-disk"])       { dumpToDisk("logs/requests", id, rawReq); }

                // forward to actual endpoint
                const options = {
                    hostname: endpointUrl.hostname,
                    port:     endpointUrl.port,
                    path:     req.url,
                    method:   req.method,
                    headers:  { ...req.headers, host: endpointUrl.host }
                };

                const proxyReq = clientLib.request(
                    options,
                    proxyRes => {
                        
                        const chunksRes = [];

                        // collect upstream response body
                        proxyRes.on("data", chunk => chunksRes.push(chunk));
                        
                        proxyRes.on(
                            "end",
                            () => {
                                
                                const bodyRes = Buffer.concat(chunksRes).toString();
                                
                                const rawRes  = [
                                    `HTTP/${proxyRes.httpVersion} ${proxyRes.statusCode}`,
                                    ...Object.entries(proxyRes.headers).map(([k, v]) => `${k}: ${v}`),
                                    "",
                                    bodyRes
                                ].join("\r\n");

                                // log response
                                if (argv["responses-to-console"])   { dumpToConsole(`RESPONSE [${id}]`, rawRes); }
                                if (argv["responses-to-disk"])      { dumpToDisk("logs/responses", id, rawRes); }

                                // send back to original (local) client
                                res.writeHead(proxyRes.statusCode, proxyRes.headers);
                                res.end(bodyRes);
                                
                            }
                        );
                        
                    }
                );

                proxyReq.on(
                    "error",
                    err => {
                        console.error("Proxy request error:", err);
                        res.writeHead(502);
                        res.end("Bad Gateway");
                    }
                );

                // write request body and finish
                proxyReq.write(bodyReq);
                proxyReq.end();
                
            }
        );

    }
).listen(
    argv.port,
    () => { console.log(`Proxy listening on port ${argv.port}; forwarding to ${argv.endpoint}`); }
);