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
    5: {"name": "Tokyo Pop",         "region": "Asia-East"}
}

def compile_binary():
    binary = "./network_engine.exe" if sys.platform == "win32" else "./network_engine"
    if not os.path.exists(binary):
        subprocess.run(["gcc", "network_engine.c", "-o", binary], check=True)
    return binary

@app.route("/", methods=["GET"])
def index():
    return render_template("dashboard.html", nodes=NODE_INFO)

@app.route("/route-traffic", methods=["POST"])
def route_traffic():
    try:
        data = request.json
        src, dest, dead = str(data.get("source")), str(data.get("destination")), str(data.get("dead_node", -1))
        binary = compile_binary()
        
        process = subprocess.Popen([binary, src, dest, dead], stdout=subprocess.PIPE, text=True)
        stdout, _ = process.communicate()
        
        if "RESULT" in stdout:
            parts = stdout.strip().split("|")
            latency = int(parts[1])
            path_nodes = [int(x) for x in parts[2].split(",")]
            path_names = [NODE_INFO[nid]["name"] for nid in path_nodes]
            return jsonify({
                "success": True,
                "latency": f"{latency} ms",
                "path_nodes": path_nodes,
                "path_display": " → ".join(path_names),
                "throughput": f"{max(40, 1000 // latency)} Gbps"
            })
        return jsonify({"success": False, "error": "Engine routing exception"})
    except Exception as e:
        return jsonify({"success": False, "error": str(e)})
if __name__ == "__main__":
    # This ensures the app binds to the cloud environment port dynamically
    port = int(os.environ.get("PORT", 5000))
    app.run(host="0.0.0.0", port=port)