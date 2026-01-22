from setuptools import setup, Extension
from Cython.Build import cythonize

# Zega Engine Extension Configuration
ext = Extension(
    "zega_engine",
    sources=[
        "bridge.pyx", 
        "core.cpp", 
        "collisions.cpp", 
        "particles.cpp"
    ],
    language="c++",
    extra_compile_args=["-O3", "-std=c++11", "-fpermissive"],
)

setup(
    name="ZEGA-X Nitro Engine",
    ext_modules=cythonize(ext),
)