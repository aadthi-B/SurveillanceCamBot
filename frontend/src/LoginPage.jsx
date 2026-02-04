import React, { useState } from "react";
import { useNavigate } from "react-router-dom";
import "./LoginPage.css"; 

const LoginPage = () => {
  const navigate = useNavigate();

  const [username, setUsername] = useState("");
  const [password, setPassword] = useState("");
  const [error, setError] = useState("");

  const handleLogin = (e) => {
    e.preventDefault();

    
    const correctUsername = "adthi";
    const correctPassword = "12345678";

    if (username === correctUsername && password === correctPassword) {
      setError("");
      navigate("/home");
    } else {
      setError("Wrong username or password ,pls correct it");
    }
  };

  return (
    <div className="login-container">
      <h2>Login</h2>
      <form onSubmit={handleLogin}>
        <input
          type="text"
          placeholder="Username"
          value={username}
          onChange={(e) => setUsername(e.target.value)}
          required
        />
        <input
          type="password"
          placeholder="Password"
          value={password}
          onChange={(e) => setPassword(e.target.value)}
          required
        />
        <button type="submit">OK</button>
        {error && <p className="error">{error}</p>}
      </form>
    </div>
  );
};

export default LoginPage;
