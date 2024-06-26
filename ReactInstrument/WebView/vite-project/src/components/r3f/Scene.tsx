import { FC } from 'react';
import { Canvas } from '@react-three/fiber';
import { OrbitControls } from '@react-three/drei'
import { Color } from 'three';
import { Cube } from './Cube';
import { Sphere } from './Sphere';
import { Cone } from './Cone';
import { Torus } from './Torus'
// @ts-ignore
import { Perf } from 'r3f-perf'

export const R3FScene: FC = () => {
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
        args={[bgColor.r, bgColor.g, bgColor.b]}
      />
      <Cube 
      // @ts-ignore
      position={[-2.2, 1.2, -1.0]}/>
      <Sphere 
      // @ts-ignore
      position={[2.2, 1.2, -1.0]}/>
      <Cone
      // @ts-ignore
      position={[-2.2, -1.2, -1.0]}/>
      <Torus
      // @ts-ignore
      position={[2.2, -1.2, -1.0]}/>
      <OrbitControls />
      <ambientLight args={[0xffffff]} intensity={0.2} />
      <directionalLight position={[0.3, 0.4, 1]} intensity={0.8} />
    </Canvas>
  );
};