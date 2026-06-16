"""
Common utilities for scripts
"""


def log_info(message, end="\n"):
    "Log info message"

    bold_font_start = "\x1b[1;39;49m"
    bold_font_stop = "\x1b[0m"

    print(bold_font_start + message + bold_font_stop, end=end)


def log_error(message, end="\n"):
    "Log error message"

    red_color_start = "\x1b[1;31;49m"
    red_color_stop = "\x1b[0m"

    print(red_color_start + message + red_color_stop, end=end)


def log_success(message, end="\n"):
    "Log success message"

    green_color_start = "\x1b[1;32;49m"
    green_color_end = "\x1b[0m"

    print(green_color_start + message + green_color_end, end=end)
