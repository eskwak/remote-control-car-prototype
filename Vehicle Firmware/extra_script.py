Import("env")
import os

# Path to the HTML file in Vehicle Controller
controller_dir = os.path.join(env["PROJECT_DIR"], "..", "Vehicle Controller", "src")
html_file = os.path.join(controller_dir, "index.html")

# Create a header file with the HTML content as a string
def create_html_header():
    if os.path.exists(html_file):
        with open(html_file, 'r', encoding='utf-8') as f:
            html_content = f.read()
        
        # Use raw string literal with a unique delimiter
        # The delimiter must not appear in the HTML content
        # Using a very unlikely sequence
        delimiter = "HTML_CONTENT_DELIMITER_XYZ123"
        
        # Check if delimiter appears in content (very unlikely)
        if delimiter in html_content:
            # Fallback: escape quotes and newlines for regular string
            html_content = html_content.replace('\\', '\\\\')
            html_content = html_content.replace('"', '\\"')
            html_content = html_content.replace('\n', '\\n')
            header_content = f'''#ifndef WEB_APP_HTML_H
#define WEB_APP_HTML_H

const char* WEB_APP_HTML = "{html_content}";

#endif
'''
        else:
            # Use raw string literal
            header_content = f'''#ifndef WEB_APP_HTML_H
#define WEB_APP_HTML_H

const char* WEB_APP_HTML = R"{delimiter}({html_content}){delimiter}";

#endif
'''
        
        header_file = os.path.join(env["PROJECT_DIR"], "include", "web_app_html.h")
        os.makedirs(os.path.dirname(header_file), exist_ok=True)
        
        with open(header_file, 'w', encoding='utf-8') as f:
            f.write(header_content)
        
        print(f"Generated {header_file} from {html_file}")
    else:
        print(f"Warning: {html_file} not found!")

# Run before building
env.AddPreAction("buildprog", create_html_header)

