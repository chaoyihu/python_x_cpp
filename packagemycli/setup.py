from setuptools import Extension, setup

setup(
    ext_modules=[
        Extension(
            name="mycli",  # as it would be imported
                           # may include packages/namespaces separated by `.`
            sources=["myclimodule.c"], # all sources are compiled into a single binary file
        ),
    ]
)
