import os
import sys
import subprocess
from flask import Flask, render_template, jsonify, request

app = Flask(__name__)

NODE_INFO = {
    0: {"name": "San Francisco Edge", "region": "US-West"},
    1: {"name": "New York Edge",     "region": "US-East"},
    2: {"name": "London Gateway",    "region": "UK-Europe"},
    3: {"name": "Frankfurt Hub",     "region": "EU-Central"},
    4: {"name": "Bengaluru Core",    "region": "Asia-South"},
    5: {"name": "Tokyo Pop",         "region": "Asia-East"},
    6: {"name": "Sydney Relay",      "region": "Oceania"},
    7: {"name": "São Paulo Link",    "region": "SA-East"},
    8: {"name": "Cape Town Base",    "region": "AF-South"},
    9: {"name": "Dubai Gateway",     "region": "ME-Central"}
}

def get_binary():
    binary = "./network_engine"
    # Linux-specific fix: Ensure binary exists and is executable
    if not os.path.exists(binary):
        try:
            subprocess.run(["gcc", "network_engine.c", "-o", binary], check=True)
            os.chmod(binary, 0o755)  # Set execution permissions
        except subprocess.CalledProcessError as e:
            print(f"Compilation failed: {e}")
    return binary

@app.route("/", methods=["GET"])
def index():
    return render_template("dashboard.html", nodes=NODE_INFO)

@app.route("/route-traffic", methods=["POST"])
def route_traffic():
    try:
        data = request.json
        src = str(data.get("source", 0))
        dest = str(data.get("destination", 4))
        dead = str(data.get("dead_node", -1))
        
        binary = get_binary()
        
        # Using check=True to catch errors during execution
        process = subprocess.run([binary, src, dest, dead], capture_output=True, text=True)
        stdout = process.stdout
        
        if "RESULT" in stdout:
            parts = stdout.strip().split("|")
            latency = int(parts[1])
            # The C code should output comma-separated nodes in parts[2]
            path_nodes = [int(x) for x in parts[2].split(",")]
            # Filter out nodes not in our dictionary
            path_names = [NODE_INFO.get(nid, {"name": "Unknown"})["name"] for nid in path_nodes]
            
            return jsonify({
                "success": True,
                "latency": f"{latency} ms",
                "path_nodes": path_nodes,
                "path_display": " → ".join(path_names),
                "throughput": f"{max(40, 1000 // (latency + 1))} Gbps"
            })
        return jsonify({"success": False, "error": "Routing engine produced invalid output"})
    except Exception as e:
        return jsonify({"success": False, "error": str(e)})

if __name__ == "__main__":
    # Ensure port is pulled from environment for Render
    port = int(os.environ.get("PORT", 5000))
    app.run(host="0.0.0.0", port=port)
