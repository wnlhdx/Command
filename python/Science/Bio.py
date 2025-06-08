# 基因组模型
class Gene:
    def __init__(self, name, sequence):
        self.name = name
        self.sequence = sequence.upper()

    def transcribe(self):
        return self.sequence.replace('T', 'U')

    def translate(self):
        codon_table = {
            'AUG': 'M', 'UUU': 'F', 'UAA': '*',
            # 可逐步扩展完整的密码子表
        }
        rna = self.transcribe()
        protein = []
        for i in range(0, len(rna), 3):
            codon = rna[i:i+3]
            aa = codon_table.get(codon, '-')
            if aa == '*':
                break
            protein.append(aa)
        return ''.join(protein)

gene1 = Gene("ATPase", "ATGTTTTAA")
rna = gene1.transcribe()
protein = gene1.translate()

print("RNA:", rna)
print("Protein:", protein)
