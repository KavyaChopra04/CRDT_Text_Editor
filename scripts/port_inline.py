import re
import os
def parse_env_file(env_file_path):
    """Parse the .env file into a dictionary."""
    env_dict = {}
    with open(env_file_path, 'r') as file:
        for line in file:
            # Ignore comments and blank lines
            line = line.strip()
            if line and not line.startswith('#'):
                key, value = line.split('=', 1)
                env_dict[key.strip()] = value.strip()
    return env_dict

def substitute_ports(input_file_path, output_file_path, env_dict):
    """Substitute the values in the input file using the mappings from env_dict."""
    with open(input_file_path, 'r') as file:
        content = file.read()

    # Replace placeholders with their corresponding values from env_dict
    for key, value in env_dict.items():
        content = re.sub(rf'\b{re.escape(key)}\b', value, content)

    # Write the updated content to the output file
    with open(output_file_path, 'w') as file:
        file.write(content)

if __name__ == "__main__":
    ports_hpp_path = os.path.realpath(os.path.join(os.getcwd(), "../backend/headers/ports.hpp")) # Input file
    ports_env_path = os.path.realpath(os.path.join(os.getcwd(), "../ports.env"))  # Environment file
    main_ports_hpp_path = os.path.realpath(os.path.join(os.getcwd(), "../backend/headers/mainports.hpp"))  # Output file

    # Parse the ports.env file
    env_dict = parse_env_file(ports_env_path)

    # Substitute values and create mainports.hpp
    substitute_ports(ports_hpp_path, main_ports_hpp_path, env_dict)

    print(f"Created {main_ports_hpp_path} with substituted values.")
