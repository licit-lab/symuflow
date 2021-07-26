# SymuFlow

Flow engine for traffic simulation.

## Installation

### From sources:

Create the conda environnement:

```bash
conda env create -f conda/env.yaml
```

Activate it and install the sources with CMake:
```bash
conda activate symuflow
mkdir build
cd build
cmake .. -DCMAKE_PREFIX_PATH=$CONDA_PREFIX -DCMAKE_INSTALL_PREFIX=$CONDA_PREFIX
make -j install
```

### From Conda:

Inside your environnement:
```bash
conda install -c licit-lab -c conda-forge symuflow
```
