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

def substitute_port_in_html(html_file_path, env_dict, output_file_template):
    """Substitute PORT in HTML file and create output files."""
    with open(html_file_path, 'r') as file:
        content = file.read()
    counter = 1
    # Generate files for specific ports
    for key in ['ALICE_FRONTEND_ALICE_BACKEND_VALUE', 'BOB_FRONTEND_BOB_BACKEND_VALUE']:
        if key in env_dict:
            substituted_content = content.replace('PORT_VALUE', env_dict[key])
            output_file_path = output_file_template.format(str(counter))
            counter += 1
            with open(output_file_path, 'w') as output_file:
                output_file.write(substituted_content)
            print(f"Created file: {output_file_path}")

if __name__ == "__main__":
    ports_env_path = os.path.realpath(os.path.join(os.getcwd(), "../ports.env"))  # Environment file
    html_file_path = os.path.realpath(os.path.join(os.getcwd(), "../frontend/clientgen.html"))  # HTML file with PORT placeholder
    output_file_template = os.path.realpath(os.path.join(os.getcwd(), "../frontend/client_{}.html"))  # Output file template

    # Parse the ports.env file
    env_dict = parse_env_file(ports_env_path)

    # Substitute PORT in clientgen.html and create output files
    substitute_port_in_html(html_file_path, env_dict, output_file_template)
