# .cmake-format.py - CMake formatting configuration for GstStudio
# Python configuration file (more reliable than YAML)

# General formatting options
format = {
    # Use spaces instead of tabs
    'tab_size': 4,
    'use_tabchars': False,

    # Line width for wrapping
    'line_width': 100,

    # Separate control flow keywords from their parentheses
    'separate_ctrl_name_with_space': False,

    # Separate function names from their parentheses
    'separate_fn_name_with_space': False,

    # Dangle parentheses for function calls
    'dangle_parens': False,

    # Allow dangling parentheses for certain commands
    'dangle_align': 'prefix',

    # Min prefix chars to trigger dangle alignment
    'min_prefix_chars': 4,

    # Max prefix chars before forcing dangle
    'max_prefix_chars': 10,

    # Max lines before forcing dangle
    'max_lines_hwrap': 2,

    # Line ending style
    'line_ending': 'unix',

    # Enable comment formatting
    'enable_comment_formatting': True,

    # Disable sorting of argument lists
    'enable_sort': False,

    # Autosort simple argument lists
    'autosort': False
}

# Parsing options
parse = {
    # Additional variable names to recognize
    'additional_commands': {
        'qt6_add_executable': {
            'pargs': {
                'nargs': '+',
                'flags': []
            },
            'kwargs': {
                'OUTPUT_DIRECTORY': 1,
                'RESOURCE_PREFIX': 1
            }
        },
        'qt6_add_qml_module': {
            'pargs': {
                'nargs': 1,
                'flags': []
            },
            'kwargs': {
                'URI': 1,
                'VERSION': 1,
                'QML_FILES': '+',
                'RESOURCES': '+',
                'SOURCES': '+'
            }
        },
        'qt6_target_qml_sources': {
            'pargs': {
                'nargs': 1,
                'flags': []
            },
            'kwargs': {
                'QML_FILES': '+'
            }
        }
    }
}

# Markup options
markup = {
    # Bullet list characters
    'bullet_char': '-',

    # Ordered list types
    'enum_char': '.',

    # First comment is literal (for file headers)
    'first_comment_is_literal': True,

    # Literal comment patterns
    'literal_comment_pattern': None,

    # Fence pattern for code blocks
    'fence_pattern': r'^\s*([`~]{3}[`~]*)(.*)',

    # Ruler pattern
    'ruler_pattern': r'^\s*[^\w\s]{3}.*[^\w\s]{3}',

    # Enable comment reflow
    'enable_comment_reflow': True,

    # Canonicalize hashrulers
    'canonicalize_hashrulers': True,

    # Hashruler min length
    'hashruler_min_length': 10
}

# Encoding options
encode = {
    # Emit byteorder mark
    'emit_byteorder_mark': False,

    # Input/output encoding
    'input_encoding': 'utf-8',
    'output_encoding': 'utf-8'
}

# Miscellaneous options
misc = {
    # Per-command line width overrides
    'per_command_line_width': {},

    # Commands to always wrap
    'always_wrap': []
}
