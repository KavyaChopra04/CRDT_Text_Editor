import psutil
import os

def kill_processes_by_name(names):
    """
    Kill processes whose names match any of the provided names.
    
    :param names: List of process names to kill (case-insensitive)
    """
    killed_processes = []
    for proc in psutil.process_iter(attrs=['pid', 'name']):
        try:
            process_name = proc.info['name']
            process_pid = proc.info['pid']
            
            # Check if the process name matches any in the target list (case-insensitive)
            if process_name and any(name.lower() in process_name.lower() for name in names):
                os.kill(process_pid, 9)  # SIGKILL
                killed_processes.append((process_name, process_pid))
                print(f"Killed process: {process_name} (PID: {process_pid})")
        except (psutil.NoSuchProcess, psutil.AccessDenied, psutil.ZombieProcess):
            pass
    
    if not killed_processes:
        print("No matching processes found.")
    else:
        print("Successfully killed the following processes:")
        for name, pid in killed_processes:
            print(f"- {name} (PID: {pid})")

if __name__ == "__main__":
    # Define process names to target
    target_process_names = ['alice', 'bob']
    
    # Kill the processes
    kill_processes_by_name(target_process_names)
