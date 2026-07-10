import { useState } from "react";

interface User {
    username: string;
    role: string;
}

interface FileInfo {
    name: string;
    size: number;
    modified: string;
}

function App() {

    const [username, setUsername] = useState("");
    const [password, setPassword] = useState("");

    const [message, setMessage] = useState("");

    const [user, setUser] = useState<User | null>(null);

    const [files, setFiles] = useState<FileInfo[]>([]);

    async function login() {

        setMessage("");

        try {

            const response = await fetch(
                "http://localhost:8000/auth/login",
                {
                    method: "POST",
                    headers: {
                        "Content-Type": "application/json"
                    },
                    body: JSON.stringify({
                        username,
                        password
                    })
                }
            );

            const data = await response.json();

            if (!response.ok) {
                setMessage(data.detail);
                return;
            }

            setUser(data);

            const fileResponse = await fetch(
                "http://localhost:8000/files"
            );

            const fileData = await fileResponse.json();

            setFiles(fileData);

        }
        catch {

            setMessage("Unable to contact backend.");

        }

    }

    if (user) {

        return (

            <div style={{ padding: "40px", fontFamily: "Arial" }}>

                <h1>ELD Frontend Portal</h1>

                <p>
                    Logged in as <b>{user.username}</b> ({user.role})
                </p>

                <h2>Stored Files</h2>

                <table
                    style={{
                        borderCollapse: "collapse",
                        width: "100%"
                    }}
                >
                    <thead>

                        <tr>

                            <th
                                style={{
                                    border: "1px solid black",
                                    padding: "8px"
                                }}
                            >
                                Filename
                            </th>

                            <th
                                style={{
                                    border: "1px solid black",
                                    padding: "8px"
                                }}
                            >
                                Size (Bytes)
                            </th>

                            <th
                                style={{
                                    border: "1px solid black",
                                    padding: "8px"
                                }}
                            >
                                Last Modified
                            </th>

                        </tr>

                    </thead>

                    <tbody>

                        {files.map((file, index) => (

                            <tr key={index}>

                                <td
                                    style={{
                                        border: "1px solid black",
                                        padding: "8px"
                                    }}
                                >
                                    {file.name}
                                </td>

                                <td
                                    style={{
                                        border: "1px solid black",
                                        padding: "8px"
                                    }}
                                >
                                    {file.size}
                                </td>

                                <td
                                    style={{
                                        border: "1px solid black",
                                        padding: "8px"
                                    }}
                                >
                                    {file.modified}
                                </td>

                            </tr>

                        ))}

                    </tbody>

                </table>

            </div>

        );

    }

    return (

        <div
            style={{
                display: "flex",
                justifyContent: "center",
                alignItems: "center",
                height: "100vh",
                fontFamily: "Arial"
            }}
        >

            <div
                style={{
                    width: "350px",
                    padding: "30px",
                    border: "1px solid #ccc",
                    borderRadius: "10px"
                }}
            >

                <h2>ELD Frontend Login</h2>

                <input
                    type="text"
                    placeholder="Username"
                    value={username}
                    onChange={(e) => setUsername(e.target.value)}
                    style={{
                        width: "100%",
                        marginBottom: "15px",
                        padding: "10px",
                        boxSizing: "border-box"
                    }}
                />

                <input
                    type="password"
                    placeholder="Password"
                    value={password}
                    onChange={(e) => setPassword(e.target.value)}
                    style={{
                        width: "100%",
                        marginBottom: "20px",
                        padding: "10px",
                        boxSizing: "border-box"
                    }}
                />

                <button
                    onClick={login}
                    style={{
                        width: "100%",
                        padding: "10px",
                        cursor: "pointer"
                    }}
                >
                    Login
                </button>

                <p
                    style={{
                        color: "darkred",
                        marginTop: "20px"
                    }}
                >
                    {message}
                </p>

            </div>

        </div>

    );

}

export default App;