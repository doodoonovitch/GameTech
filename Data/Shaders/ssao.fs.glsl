float ComputeSSAO(
	// 4×4 texture containing 16 random vectors
	sampler2D sRotSampler4x4,
	// scene depth target containing normalized
	// from 0 to 1 linear depth
	sampler2D sSceneDepthSampler,
	// texture coordinates of current pixel
	float2 screenTC,
	// dimensions of the screen
	float2 screenSize,
	// far clipping plane distance in meters
	float farClipDist)
{
	//get rotation vector, rotation is tiled every 4 screen pixels
	float2 rotationTC = screenTC * screenSize / 4;
	float3 vRotation = 2 * tex2D(sRotSampler4x4, rotationTC).rgb - 1;
	
	// create rotation matrix from rotation vector
	float3x3 rotMat;
	float h = 1 / (1 + vRotation.z );
	rotMat._m00 =  h * vRotation.y * vRotation.y + vRotation.z;
	rotMat._m01 = -h * vRotation.y * vRotation.x;
	rotMat._m02 = -vRotation.x;
	rotMat._m10 = -h * vRotation.y * vRotation.x;
	rotMat._m11 =  h * vRotation.x * vRotation.x + vRotation.z ;
	rotMat._m12 = -vRotation.y; 
	rotMat._m20 = vRotation.x;
	rotMat._m21 = vRotation.y; 
	rotMat._m22 = vRotation.z;

	// get depth of current pixel and convert into meters
	float fSceneDepthP = tex2D( sSceneDepthSampler, screenTC ).r * farClipDist;
	
	// parameters affecting offset points number and distribution
	const int nSamplesNum = 16; // may be 8, 16 or 24
	float offsetScale = 0.01;
	const float offsetScaleStep = 1 + 2.4 / nSamplesNum;
	float Accessibility = 0;
	
	// sample area and accumulate accessibility
	for(int i=0; i<(nSamplesNum/8); i++)
		for(int x=-1; x<=1; x+=2)
			for(int y=-1; y<=1; y+=2)
				for(int z =-1; z <=1; z +=2) 
				{
					// generate offset v ector (this code line is executed only
					// at shader compile stage)
					// here we use cube corners and giv e it different lengths
					float3 vOffset = normalize(float3( x, y, z )) * ( offsetScale *= offsetScaleStep );
					// rotate offset v ector by rotation matrix
					float3 vRotatedOffset = mul( vOffset, rotMat );
					// get center pixel 3d coordinates in screen space
					float3 vSamplePos = float3( screenTC, fSceneDepthP );
					// shift coordinates by offset v ector (range conv ert
					// and width depth v alue)
					vSamplePos += float3( vRotatedOffset.xy, vRotatedOffset.z * fSceneDepthP * 2);
					// read scene depth at sampling point and conv ert into meters
					float fSceneDepthS = tex2D( sSceneDepthSampler, vSamplePos.xy ) * farClipDist;
					// check if depths of both pixels are close enough and
					// sampling point should affect our center pixel
					float fRangeIsInvalid = saturate( ( ( fSceneDepthP - fSceneDepthS ) / fSceneDepthS ) );
					// accumulate accessibility, use default v alue of 0.5
					// if right computations are not possible
					Accessibility += lerp( fSceneDepthS > vSamplePos.z , 0.5, fRangeIsInv alid );
				} 
	//get average value
	Accessibility = Accessibility / nSamplesNum;
	// amplify and saturate if necessary
	return saturate( Accessibility * Accessibility + Accessibility );
}