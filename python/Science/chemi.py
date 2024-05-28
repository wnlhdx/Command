from rdkit import Chem
from rdkit.Chem import AllChem

# 创建一个分子
mol = Chem.MolFromSmiles('CCCC')  # 假设这是一个四碳烷烃

# 定义一个反应
# 这里我们假设有一个简单的加成反应
# 添加一个溴原子到分子上
AllChem.EmbedMolecule(mol)
AllChem.MMFFOptimizeMolecule(mol)

# 绘制分子结构
Chem.MolToMolBlock(mol, None, False, True, False, False, True)

# 输出优化后的分子结构
print(Chem.MolToSmiles(mol))
