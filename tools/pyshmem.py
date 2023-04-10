from multiprocessing import shared_memory
import threading

# Create a named semaphore with initial value of 1
semaphore = threading.Semaphore(value=1)

# Create a shared memory block of size 100 bytes
shm = shared_memory.SharedMemory(name='my_shm', create=True, size=100)

# Write data to the shared memory block
data = b'Hello, world!'
with semaphore:
    shm.buf[:len(data)] = data

x = input("Press any key to exit")

# Close and unlink the shared memory block
shm.close()
shm.unlink()
