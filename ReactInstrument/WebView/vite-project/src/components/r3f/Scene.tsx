import { FC } from 'react';
import { Canvas } from '@react-three/fiber';
import { OrbitControls } from '@react-three/drei'
import { Color } from 'three';
import { Cube } from './Cube';
// import { Sphere } from './Sphere';
// import { Cone } from './Cone';
// import { Torus } from './Torus'
// @ts-ignore
import { Perf } from 'r3f-perf'

// @ts-ignore
export const R3FScene: FC = (props: any) => {
  const bgColor = new Color("darkgray");
  
  return (
    <Canvas
      camera={{
        fov: 45,
        near: 0.1,
        far: 1000,
        position: [0, 0, 5]
      }}
    >
      {/* <Perf/> */}
      <color
        attach="background"
        // @ts-ignore
        args={[props.knobValue, bgColor.g, bgColor.b]}
      />
      <Cube 
      // @ts-ignore
      position={[5, 0, -1.0]}
      // @ts-ignore
      scale={props.knobValue * 2 + 0.2}/>
      <OrbitControls />
      <ambientLight args={[0xffffff]} intensity={0.2} />
      <directionalLight position={[0.3, 0.4, 1]} intensity={0.8} />
    </Canvas>
  );
};