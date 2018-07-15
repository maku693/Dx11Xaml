float4 main(float3 position : SV_Position) : SV_Position {
  return float4(position, 1);
}
