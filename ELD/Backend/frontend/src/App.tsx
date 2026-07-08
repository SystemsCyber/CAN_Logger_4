import { useEffect, useState } from "react";

interface HealthResponse {
  status: string;
  version: string;
}

function App() {
  const [health, setHealth] = useState<HealthResponse | null>(null);
  const [loading, setLoading] = useState(true);

  useEffect(() => {
    fetch("http://localhost:8000/health")
      .then((res) => res.json())
      .then((data) => {
        setHealth(data);
        setLoading(false);
      })
      .catch(() => {
        setLoading(false);
      });
  }, []);

  return (
    <div
      style={{
        fontFamily: "Arial, sans-serif",
        maxWidth: "800px",
        margin: "50px auto",
      }}
    >
      <h1>ELD Backend Portal</h1>

      <hr />

      <h2>System Status</h2>

      {loading ? (
        <p>Checking backend...</p>
      ) : health ? (
        <>
          <p>
            <strong>Status:</strong> {health.status}
          </p>
          <p>
            <strong>Version:</strong> {health.version}
          </p>
        </>
      ) : (
        <p style={{ color: "red" }}>
          Backend Offline
        </p>
      )}
    </div>
  );
}

export default App;