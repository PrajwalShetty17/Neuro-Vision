import os
import sys
import subprocess
from flask import Flask, render_template, jsonify, request

app = Flask(__name__)

# Compile C binary once at startup
def build_c_engine():
    binary_name = "network_engine"
    # Clean previous binary if exists
    if os.path.exists(binary_name):
        os.remove(binary_name)
    try:
        subprocess.run(["gcc", "-o", binary_name, "network_engine.c"], check=True)
        os.chmod(binary_name, 0o755)
        print("C Engine compiled and set to executable.")
    except Exception as e:
        print(f"Critical Compilation Error: {e}")

@app.route("/", methods=["GET"])
def index():
    # Basic node data for the frontend
    nodes = {0: {"name": "SF", "region": "US"}, 1: {"name": "NY", "region": "US"}, 
             2: {"name": "London", "region": "EU"}, 3: {"name": "Frankfurt", "region": "EU"}}
    return render_template("dashboard.html", nodes=nodes)

@app.route("/route-traffic", methods=["POST"])
def route_traffic():
    data = request.json
    src = str(data.get("source"))
    dest = str(data.get("destination"))
    dead = str(data.get("dead_node", "-1"))
    
    try:
        # Execute binary: ./network_engine <src> <dest> <dead_node>
        result = subprocess.run(["./network_engine", src, dest, dead], 
                                capture_output=True, text=True, check=True)
        return jsonify({"output": result.stdout.strip(), "success": True})
    except Exception as e:
        return jsonify({"success": False, "error": str(e)})

if __name__ == "__main__":
    build_c_engine()
    port = int(os.environ.get("PORT", 5000))
    app.run(host="0.0.0.0", port=port)
