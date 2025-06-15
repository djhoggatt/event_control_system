import pytest
import serial


def pytest_addoption(parser):
    parser.addoption("--port", help="The port of the device.")


@pytest.fixture(scope="session")
def port(request):
    return request.config.getoption("--port")


class Device:
    def __init__(self, port):
        self.serial_device = serial.Serial(port, 115200, timeout=3, write_timeout=1)

    def write(self, string_to_write: str):
        self.serial_device.write(string_to_write.encode("utf-8"))

    def write_bytes(self, bytes_to_write: bytearray):
        self.serial_device.write(bytes_to_write)

    def read(self, expected: bytearray = b">") -> str:
        return self.serial_device.read_until(expected).decode("utf-8")


@pytest.fixture(scope="session")
def device(port):
    return Device(port)
