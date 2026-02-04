import React, { useEffect, useRef, useState } from "react";
import "./App.css";

const App = () => {
  const socketRef = useRef(null);
  const [isConnected, setIsConnected] = useState(false);

  useEffect(() => {
    const socket = new WebSocket("ws://192.168.4.1:81");

    socket.onopen = () => {
      console.log("WebSocket connected");
      setIsConnected(true);
    };

    socket.onclose = () => {
      console.log("WebSocket disconnected");
      setIsConnected(false);
    };

    socket.onerror = (error) => {
      console.error("WebSocket error:", error);
    };

    socketRef.current = socket;

    return () => {
      socket.close();
    };
  }, []);

  const sendCommand = (cmd) => {
    if (socketRef.current && socketRef.current.readyState === WebSocket.OPEN) {
      socketRef.current.send(cmd);
    } else {
      console.warn("WebSocket not connected.");
    }
  };

  return (
    <div className="app">
      <h1>Surveillance Bot Controller</h1>

      {!isConnected && <p className="warning">Connecting to bot...</p>}

      <div className="video-container">
        <img
          src="http://192.168.147.242:4747/video"
          alt="Live Stream"
          className="video-stream"
        />
      </div>

      <div className="controls-section">
        {/* Motor Control */}
        <div className="control-group">
          <h2>Movement Control</h2>
          <div className="control-grid motor">
            <button onClick={() => sendCommand("forward")}>↑</button>
            <button onClick={() => sendCommand("left")}>←</button>
            <button onClick={() => sendCommand("stop")}>■</button>
            <button onClick={() => sendCommand("right")}>→</button>
            <button onClick={() => sendCommand("backward")}>↓</button>
          </div>
        </div>

        {/* Pan-Tilt Control */}
        <div className="control-group">
          <h2>Pan-Tilt Control</h2>
          <div className="control-grid pan-tilt">
            <button onClick={() => sendCommand("tilt_down")}>↑</button>
            <button onClick={() => sendCommand("pan_left")}>←</button>
            <button onClick={() => sendCommand("pan_right")}>→</button>
            <button onClick={() => sendCommand("tilt_up")}>↓</button>
          </div>
        </div>
      </div>
    </div>
  );
};

export default App;