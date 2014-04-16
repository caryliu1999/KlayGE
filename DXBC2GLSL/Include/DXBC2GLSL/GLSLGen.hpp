#ifndef _DXBC2GLSL_GLSLGEN_HPP
#define _DXBC2GLSL_GLSLGEN_HPP

#pragma once

#include <DXBC2GLSL/Shader.hpp>
#include <map>

enum GLSLVersion
{
	GSV_110 = 0,	// GL 2.0
	GSV_120,		// GL 2.1
	GSV_130,		// GL 3.0
	GSV_140,		// GL 3.1
	GSV_150,		// GL 3.2
	GSV_330,		// GL 3.3
	GSV_400,		// GL 4.0
	GSV_410,		// GL 4.1
	GSV_420,		// GL 4.2
	GSV_430,		// GL 4.3
	GSV_440,		// GL 4.4

	GSV_100_ES,		// GL ES 2.0
	GSV_300_ES,		// GL ES 3.0
	GSV_310_ES		// GL ES 3.1
};

enum GLSLRules
{
	GSR_UniformBlockBinding = 1UL << 0,		// Set means allow uniform block layout bindings e.g.layout(binding=N) uniform {};.
	GSR_GlobalUniformsInUBO = 1UL << 1,		// Set means collect global uniforms in uniform block named $Globals.
	GSR_UseUBO = 1UL << 2,					// Set means generating uniform blocks.
	GSR_ExplicitPSOutputLayout = 1UL << 3,
	GSR_ExplicitInputLayout = 1UL << 4,
	GSR_UIntType = 1UL << 5,
	GSR_GenericTexture = 1UL << 6,
	GSR_PSInterpolation = 1UL << 7,
	GSR_InOutPrefix = 1UL << 8,
	GSR_Int64Type = 1UL << 9,
	GSR_TextureGrad = 1UL << 10,
	GSR_BitwiseOp = 1UL << 11,
	GSR_MultiStreamGS = 1UL << 12,
	GSR_CoreGS = 1UL << 13,
	GSR_Precision = 1UL << 14,
	GSR_VersionDecl = 1UL << 14,
	GSR_ForceUInt32 = 0xFFFFFFFF
};

struct RegisterDesc
{
	uint32_t index;
	ShaderRegisterComponentType type;
	ShaderInterpolationMode interpolation;
	bool is_depth;
};

struct HSForkPhase
{
	uint32_t fork_instance_count;
	std::vector<KlayGE::shared_ptr<ShaderDecl>> dcls;
	std::vector<KlayGE::shared_ptr<ShaderInstruction> > insns;//instructions
	
	HSForkPhase()
		:fork_instance_count(0){}
};

struct HSControlPointPhase
{
	std::vector<KlayGE::shared_ptr<ShaderDecl>> dcls;
	std::vector<KlayGE::shared_ptr<ShaderInstruction> > insns;//instructions
};

class GLSLGen
{
public:
	static uint32_t DefaultRules(GLSLVersion version);

	void FeedDXBC(KlayGE::shared_ptr<ShaderProgram> const & program, bool has_gs, GLSLVersion version, uint32_t glsl_rules);
	void ToGLSL(std::ostream& out);
	void ToHSForkPhases(std::ostream& out);
	void ToHSControlPointPhase(std::ostream& out);

private:
	void ToDeclarations(std::ostream& out);
	void ToDclInterShaderInputRecords(std::ostream& out);
	void ToDclInterShaderOutputRecords(std::ostream& out);
	void ToDclInterShaderPatchConstantRecords(std::ostream& out);
	void ToDeclInterShaderInputRegisters(std::ostream& out) const;
	void ToCopyToInterShaderInputRegisters(std::ostream& out) const;
	void ToDeclInterShaderOutputRegisters(std::ostream& out) const;
	void ToCopyToInterShaderOutputRecords(std::ostream& out) const;
	void ToDclInterShaderPatchConstantRegisters(std::ostream& out);
	void ToCopyToInterShaderPatchConstantRecords(std::ostream& out)const;
	void ToCopyToInterShaderPatchConstantRegisters(std::ostream& out)const;
	void ToDefaultHSControlPointPhase(std::ostream& out)const;
	void ToDeclaration(std::ostream& out, ShaderDecl const & dcl);
	void ToInstruction(std::ostream& out, ShaderInstruction const & insn) const;
	void ToOperands(std::ostream& out, ShaderOperand const & op, uint32_t imm_as_type,
		bool mask = true, bool dcl_array = false, bool no_swizzle = false, bool no_idx = false, bool no_cast = false) const;
	ShaderImmType OperandAsType(ShaderOperand const & op, uint32_t imm_as_type) const;
	int ToSingleComponentSelector(std::ostream& out, ShaderOperand const & op, int i, bool dot = true) const;
	void ToOperandName(std::ostream& out, ShaderOperand const & op, ShaderImmType as_type,
		bool* need_idx, bool* need_comps, bool no_swizzle = false, bool no_idx = false) const;
	void ToComponentSelectors(std::ostream& out, ShaderOperand const & op, bool dot = true) const;
	void ToTemps(std::ostream& out, ShaderDecl const & dcl);
	void ToDefaultValue(std::ostream& out, DXBCShaderVariable const & var);
	void ToDefaultValue(std::ostream& out, DXBCShaderVariable const & var, uint32_t offset);
	void ToDefaultValue(std::ostream& out, char const * value, ShaderVariableType type);
	uint32_t ComponentSelectorFromMask(uint32_t mask, uint32_t comps) const;
	uint32_t ComponentSelectorFromSwizzle(uint8_t const swizzle[4], uint32_t comps) const;
	uint32_t ComponentSelectorFromScalar(uint8_t scalar) const;
	uint32_t ComponentSelectorFromCount(uint32_t count) const;
	void ToComponentSelector(std::ostream& out, uint32_t comps) const;
	bool IsImmediateNumber(ShaderOperand const & op) const;
	// param i:the component selector to get
	// return:the idx of selector:0 1 2 3 stand for x y z w
	// example: for .xw i=0 return 0 i=1 return 3
	int GetComponentSelector(ShaderOperand const & op, int i) const;
	int GetOperandComponentNum(ShaderOperand const & op) const;
	uint32_t GetMaxComponentSelector(ShaderOperand const & op) const;
	uint32_t GetMinComponentSelector(ShaderOperand const & op) const;
	ShaderRegisterComponentType GetOutputParamType(ShaderOperand const & op) const;
	DXBCSignatureParamDesc const & GetOutputParamDesc(ShaderOperand const & op, uint32_t index = 0) const;
	DXBCSignatureParamDesc const & GetInputParamDesc(ShaderOperand const & op, uint32_t index = 0) const;
	DXBCInputBindDesc const & GetResourceDesc(ShaderInputType type, uint32_t bind_point) const;
	uint32_t GetNumPatchConstantSignatureRegisters(std::vector<DXBCSignatureParamDesc> const & params_patch)const;
	void FindDclIndexRange();
	void FindSamplers();
	void LinkCFInsns();
	void FindLabels();
	void FindEndOfProgram();
	void FindTempDcls();
	void FindHSForkPhases();
	void FindHSControlPointPhase();

private:
	KlayGE::shared_ptr<ShaderProgram> program_;

	ShaderType shader_type_;
	bool has_gs_;
	std::vector<DclIndexRangeInfo> idx_range_info_;
	std::vector<TextureSamplerInfo> textures_;
	std::vector<KlayGE::shared_ptr<ShaderDecl> > temp_dcls_;
	std::map<int64_t, bool> cb_index_mode_;
	std::vector<HSForkPhase> hs_fork_phases_;
	std::vector<HSControlPointPhase> hs_control_point_phase_;
	bool enter_hs_fork_phase_;
	bool enter_final_hs_fork_phase_;

	// for ifs, the insn number of the else or endif if there is no else
	// for elses, the insn number of the endif
	// for endifs, the insn number of the if
	// for loops, the insn number of the endloop
	// for endloops, the insn number of the loop
	// for all others, -1
	std::vector<uint32_t> cf_insn_linked_;
	std::vector<LabelInfo> label_to_insn_num_;
	bool labels_found_;
	// the id of ret instruction which is not nested in any flow control statements
	uint32_t end_of_program_;
	GLSLVersion glsl_version_;
	uint32_t glsl_rules_;

	mutable std::vector<uint8_t> temp_as_type_;

};

#endif		// _DXBC2GLSL_GLSLGEN_HPP
