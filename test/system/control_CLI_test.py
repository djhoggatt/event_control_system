"""
System Tests for the CLI Control Module
"""

# ------------------------------------------------------------------------------
#  Imports
# ------------------------------------------------------------------------------

import pytest

# ------------------------------------------------------------------------------
# Global Constants
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
# Helper Functions
# ------------------------------------------------------------------------------


def reset_state(device):
    device.write("\n")
    device.read()


# ------------------------------------------------------------------------------
# Tests
# ------------------------------------------------------------------------------


def test_response(device):
    device.write("\n")
    assert ">" in device.read()

    reset_state(device)


def test_help(device):
    device.write("help\n")
    assert "help: " in device.read()

    reset_state(device)


def test_up_arrow(device):
    device.write("help\n")
    device.read()
    device.write_bytes(b"\x1b\x5b\x41")
    assert "help" in device.read(b"help")

    reset_state(device)


def test_down_arrow(device):
    device.write("help")
    device.read(b"help")
    device.write_bytes(b"\x1b\x5b\x42")

    line = device.read()

    assert ">" in line
    assert "help" not in line

    reset_state(device)


def test_tab_complete(device):
    device.write("hel")
    device.read(b"hel")
    device.write_bytes(b"\x09")
    assert "help" in device.read(b"help")

    reset_state(device)


# End of File
