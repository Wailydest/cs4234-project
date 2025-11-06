import json
import subprocess
import sys
from pathlib import Path
from typing import Dict, List


def _run_command(command: List[str]) -> Dict:
    """
    Abstract command execution mechanism.

    Args:
        command: List of command arguments (executable path + arguments)

    Returns:
        Parsed JSON output as a dictionary

    Raises:
        FileNotFoundError: If executable doesn't exist
        RuntimeError: If command fails with non-zero return code
        ValueError: If output is empty or invalid JSON
    """
    executable_path = Path(command[0])

    # Check if executable exists
    if not executable_path.exists():
        raise FileNotFoundError(
            f"Executable not found at {executable_path}. "
            "Please build the project first with 'cd build && cmake .. && make'"
        )

    # Use Popen to stream stderr in real-time while capturing stdout
    process = subprocess.Popen(
        command,
        stdout=subprocess.PIPE,
        stderr=sys.stderr,  # Stream stderr directly to notebook in real-time
        text=True,
    )

    # Wait for process to complete and capture stdout
    stdout, _ = process.communicate()
    return_code = process.returncode

    # Check return code
    if return_code != 0:
        raise RuntimeError(
            f"Command failed with return code {return_code}.\n"
            "Check stderr output above for details."
        )

    # Check if stdout is empty
    if not stdout.strip():
        raise ValueError(
            "Command produced no output. Check stderr output above for details."
        )

    # Parse and return JSON
    try:
        return json.loads(stdout)
    except json.JSONDecodeError as e:
        raise ValueError(
            f"Failed to parse JSON output.\nStdout: {stdout[:200]}\nJSON Error: {e}"
        ) from e


def _get_executable_path() -> Path:
    """Get the path to the matroid_intersection executable."""
    project_root = Path(__file__).parent
    return project_root / "build" / "matroid_intersection"


def run_bipartite_matching(
    n: int, p: float, seed: int = 42, time_limit: int = 10
) -> Dict:
    """
    Run bipartite matching algorithm.

    Args:
        n: Number of vertices
        p: Edge probability
        seed: Random seed (default: 42)
        time_limit: Time limit in seconds (default: 10)

    Returns:
        Dictionary containing the JSON output from the algorithm
    """
    executable_path = _get_executable_path()
    command = [
        str(executable_path),
        "bipartite",
        str(n),
        str(p),
        str(seed),
        str(time_limit),
    ]
    return _run_command(command)


def run_3d_matching(n: int, p: float, seed: int = 42, time_limit: int = 10) -> Dict:
    """
    Run 3D matching algorithm.

    Args:
        n: Number of vertices
        p: Hyperedge probability
        seed: Random seed (default: 42)
        time_limit: Time limit in seconds (default: 10)

    Returns:
        Dictionary containing the JSON output from the algorithm
    """
    executable_path = _get_executable_path()
    command = [
        str(executable_path),
        "3dmatching",
        str(n),
        str(p),
        str(seed),
        str(time_limit),
    ]
    return _run_command(command)


def run_hamiltonian(
    n: int,
    p: float,
    min_hamiltonian_path_length: int = 0,
    seed: int = 42,
    time_limit: int = 10,
) -> Dict:
    """
    Run Hamiltonian path algorithm.

    Args:
        n: Number of vertices
        p: Edge probability
        min_hamiltonian_path_length: Minimum Hamiltonian path length (default: 0)
        seed: Random seed (default: 42)
        time_limit: Time limit in seconds (default: 10)

    Returns:
        Dictionary containing the JSON output from the algorithm
    """
    executable_path = _get_executable_path()
    command = [
        str(executable_path),
        "hamiltonian",
        str(n),
        str(p),
        str(min_hamiltonian_path_length),
        str(seed),
        str(time_limit),
    ]
    return _run_command(command)
